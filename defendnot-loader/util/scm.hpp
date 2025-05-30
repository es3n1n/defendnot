#pragma once
#include <array>
#include <memory>
#include <optional>
#include <string_view>
#include <Windows.h>

namespace scm {
    using SCHandleRaw = SC_HANDLE;
    using SCHandle = std::unique_ptr<std::remove_pointer_t<SCHandleRaw>, decltype(&CloseServiceHandle)>;

    inline SCHandle make_sc_handle(SCHandleRaw handle) {
        return SCHandle(handle, CloseServiceHandle);
    }

    enum class ServiceState : std::uint8_t {
        UNKNOWN = 0,
        STOPPED,
        STOP_PENDING,
        START_PENDIND,
        RUNNING,
        PAUSED,
        PAUSE_PENDING,
        CONTINUE_PENDING,
    };
    /// \note @es3n1n: we should use magic_enum once we have more than one enum where we need to get value name
    constexpr auto kServiceStateNames =
        std::to_array<std::string_view>({"UNKNOWN", "STOPPED", "STOP_PENDING", "START_PENDING", "RUNNING", "PAUSED", "PAUSE_PENDING", "CONTINUE_PENDING"});

    class Service {
    public:
        Service(SCHandleRaw handle): handle_(make_sc_handle(handle)) { };
        ~Service() = default;

    public:
        bool query_status() noexcept {
            if (status_process_.has_value()) {
                return true;
            }

            SERVICE_STATUS_PROCESS status;
            DWORD needed = 0;
            if (!QueryServiceStatusEx(handle_.get(), SC_STATUS_PROCESS_INFO, reinterpret_cast<PBYTE>(&status), sizeof(status), &needed)) {
                return false;
            }

            status_process_ = status;
            return true;
        }

        [[nodiscard]] ServiceState state() noexcept {
            if (!query_status() || !status_process_.has_value()) {
                return ServiceState::UNKNOWN;
            }

            switch (status_process_->dwCurrentState) {
            case SERVICE_STOPPED:
                return ServiceState::STOPPED;
            case SERVICE_STOP_PENDING:
                return ServiceState::STOP_PENDING;
            case SERVICE_START_PENDING:
                return ServiceState::START_PENDIND;
            case SERVICE_RUNNING:
                return ServiceState::RUNNING;
            case SERVICE_PAUSED:
                return ServiceState::PAUSED;
            case SERVICE_PAUSE_PENDING:
                return ServiceState::PAUSE_PENDING;
            case SERVICE_CONTINUE_PENDING:
                return ServiceState::CONTINUE_PENDING;
            default:
                return ServiceState::UNKNOWN;
            }
        }

        [[nodiscard]] bool valid() const noexcept {
            return handle_.get() != nullptr;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return valid();
        }

    private:
        std::optional<SERVICE_STATUS_PROCESS> status_process_ = std::nullopt;
        SCHandle handle_;
    };

    class Manager {
        constexpr static auto kDesiredPermissions = GENERIC_READ;

    public:
        Manager(): handle_(make_sc_handle(OpenSCManagerW(nullptr, nullptr, kDesiredPermissions))) { };
        ~Manager() = default;

    public:
        [[nodiscard]] Service get_service(const std::wstring_view service_name) noexcept {
            return Service(OpenServiceW(handle_.get(), service_name.data(), kDesiredPermissions));
        }

    public:
        [[nodiscard]] bool valid() const noexcept {
            return handle_.get() != nullptr;
        }

        [[nodiscard]] explicit operator bool() const noexcept {
            return valid();
        }

    private:
        SCHandle handle_;
    };
} // namespace scm
