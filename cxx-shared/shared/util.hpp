#pragma once
#include <filesystem>
#include <mutex>

#include <Windows.h>

namespace shared {
    inline std::filesystem::path get_this_module_path() {
        char result[_MAX_PATH] = {0};

        /// \fixme @es3n1n: This is sketchy
        HMODULE h_module = nullptr;
        GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCSTR>(&get_this_module_path), &h_module);

        GetModuleFileNameA(h_module, result, sizeof(result));
        return result;
    }

    inline void alloc_console() {
        static std::once_flag fl;

        /// Most likely the process we're injecting to does not have allocated console
        std::call_once(fl, []() -> void {
            AllocConsole();
            freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
            freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w", stderr);
        });
    }

    inline bool is_winserver() {
        OSVERSIONINFOEXW osvi = {sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0, 0, VER_NT_WORKSTATION};
        const auto cond_mask = VerSetConditionMask(0, VER_PRODUCT_TYPE, VER_EQUAL);
        return !VerifyVersionInfoW(&osvi, VER_PRODUCT_TYPE, cond_mask);
    }
} // namespace shared
