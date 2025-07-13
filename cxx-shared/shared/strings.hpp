#pragma once
#include "shared/util.hpp"
#include <string_view>

namespace strings {
    constexpr std::string_view kProjectName = "defendnot";
    constexpr std::string_view kRepoUrl = "https://github.com/es3n1n/defendnot";
    constexpr std::string_view kVersion = "1.4.0";

    constexpr std::string_view kDefaultAVName = "dnot.sh";

    constexpr std::string_view kVictimProcess = "Taskmgr.exe";
    constexpr std::string_view kDllName = "defendnot.dll";

    constexpr std::string_view kWSCUnavailableError = /// !winserver
        "Windows Security Center (WSC) is not available on this machine.\n"
        "For more details, please refer to: https://github.com/es3n1n/defendnot/issues/25";

    constexpr std::string_view kWSCUnavailableErrorWinServer = /// winserver
        "Windows Security Center (WSC) is not available on this machine.\n"
        "This typically occurs on Windows Server operating systems, which are not supported by this tool.\n"
        "For more details, please refer to: https://github.com/es3n1n/defendnot/issues/17";

    inline std::string_view wsc_unavailable_error() noexcept {
        if (shared::is_winserver()) {
            return kWSCUnavailableErrorWinServer;
        }
        return kWSCUnavailableError;
    }
} // namespace strings
