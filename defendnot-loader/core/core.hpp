#pragma once
#include <string>
#include <string_view>

#include <Windows.h>

namespace loader {
    enum class AutorunType : std::uint8_t {
        AS_SYSTEM_ON_BOOT = 0, ///< launch on system boot as NT AUTHORITY\SYSTEM
        AS_CURRENT_USER_ON_LOGIN = 1, ///< launch on user login
    };

    struct Config {
    public:
        std::string name;
        bool disable;
        bool alloc_console;
        bool verbose;
        bool from_autorun;
        AutorunType autorun_type;
        bool enable_autorun;
    };

    [[nodiscard]] HANDLE inject(std::string_view dll_path, std::string_view proc_name);
    [[nodiscard]] bool add_to_autorun(AutorunType type);
    [[nodiscard]] bool remove_from_autorun();
    void ensure_environment();
} // namespace loader
