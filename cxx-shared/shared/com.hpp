#pragma once
#include <Windows.h>

#include "shared/strings.hpp"

#include <cstdint>
#include <format>
#include <print>
#include <source_location>
#include <stdexcept>

namespace com {
    inline HRESULT checked(HRESULT result, const std::source_location loc = std::source_location::current()) {
        if (result == 0) {
            return result;
        }

        auto msg = std::format("Got HRESULT={:#x} at\n{}:{}", static_cast<std::uint32_t>(result) & 0xFFFFFFFF, loc.function_name(), loc.line());
        throw std::runtime_error(msg);
    }

    template <typename Callable>
    inline HRESULT retry_while_pending(Callable&& fn) {
        bool delayed = false;
        HRESULT status = 0;
        do {
            if (status != 0) {
                delayed = true;
                std::println("delaying for com retry...");
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }

            status = fn();
        } while (status == E_PENDING);

        return status;
    }

    /// A very basic implementation, a lot of stuff is missing
    template <typename Ty>
    class Ptr {
    public:
        Ptr() = default;
        explicit Ptr(Ty* ptr): ptr_(ptr) { }

        ~Ptr() {
            release();
        }

        /// No copying
        Ptr(const Ptr&) = delete;
        Ptr& operator=(const Ptr&) = delete;

        /// Move
        Ptr(Ptr&& other) noexcept: ptr_(other.ptr_) {
            other.ptr_ = nullptr;
        }
        Ptr& operator=(Ptr&& other) noexcept {
            if (this != &other) {
                release();
                ptr_ = other.ptr_;
                other.ptr_ = nullptr;
            }
            return *this;
        }

    public:
        [[nodiscard]] Ty* get() const {
            return ptr_;
        }

        [[nodiscard]] Ty* operator->() const {
            return ptr_;
        }

        [[nodiscard]] Ty** ref_to_ptr() {
            return &ptr_;
        }

    private:
        void release() {
            if (ptr_ != nullptr) {
                ptr_->Release();
            }
        }

        Ty* ptr_ = nullptr;
    };

    template <GUID ClsId, GUID IID>
    class IBaseObject {
    public:
        static constexpr GUID kClsId = ClsId;
        static constexpr GUID kIID = IID;

    private:
        virtual HRESULT QueryInterface() = 0;
        virtual std::uint32_t AddRef() = 0;

    public:
        virtual std::uint32_t Release() = 0;
    };

    template <typename Ty>
    concept ComObject = requires {
        Ty::kClsId;
        Ty::kIID;
    };

    template <ComObject Ty>
    [[nodiscard]] Ptr<Ty> query() {
        Ptr<Ty> result;
        const auto status = CoCreateInstance(Ty::kClsId, 0, 1, Ty::kIID, reinterpret_cast<LPVOID*>(result.ref_to_ptr()));
        if (status == REGDB_E_CLASSNOTREG) {
            throw std::runtime_error(strings::wsc_unavailable_error().data());
        }

        checked(status);
        return result;
    }
} // namespace com
