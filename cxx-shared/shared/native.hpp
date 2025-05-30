#pragma once
#include <format>
#include <stdexcept>
#include <Windows.h>

#pragma pack(push, 1)
namespace native {
    class PEB {
    public:
        std::uint8_t inherited_address_space;
        std::uint8_t read_image_file_exec_options;
        /// - we don't need other fields
    };

    static_assert(offsetof(PEB, read_image_file_exec_options) == 1);

    template <typename Ty>
    inline Ty get_system_routine(const std::string_view module_name, const std::string_view function_name) {
        const auto mod = GetModuleHandleA(module_name.data());
        if (mod == nullptr) {
            throw std::runtime_error(std::format("unable to find module {}", module_name));
        }

        auto function = reinterpret_cast<Ty>(GetProcAddress(mod, function_name.data()));
        if (function == nullptr) {
            throw std::runtime_error(std::format("unable to obtain {} from {}", function_name, module_name));
        }
        return function;
    }

    inline PEB* get_peb() {
        static auto function = get_system_routine<PEB*(__stdcall*)()>("ntdll.dll", "RtlGetCurrentPeb");
        static auto result = function();
        if (result == nullptr) [[unlikely]] {
            throw std::runtime_error("no peb");
        }

        return result;
    }

    inline bool debug_set_process_kill_on_exit(const bool value) {
        static auto function = get_system_routine<BOOL(__stdcall*)(BOOL)>("kernel32.dll", "DebugSetProcessKillOnExit");
        return static_cast<bool>(function(static_cast<BOOL>(value)));
    }

    inline bool debug_active_process_stop(const std::uint32_t process_id) {
        static auto function = get_system_routine<BOOL(__stdcall*)(DWORD)>("kernel32.dll", "DebugActiveProcessStop");
        return static_cast<bool>(function(process_id));
    }
} // namespace native
#pragma pack(pop)
