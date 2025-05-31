#include "bootstrap.hpp"
#include "core/com.hpp"
#include "core/log.hpp"
#include "shared/ctx.hpp"
#include "shared/defer.hpp"

#include <Windows.h>

namespace defendnot {
    namespace {
        template <com::ComObject Ty>
        void apply(const std::string_view log_prefix, const BSTR name) {
            /// Get the WSC interface
            auto inst = com::query<Ty>();

            /// This can fail if we dont have any products registered so no com_checked
            logln("{}_unregister: {:#x}", log_prefix, com::retry_while_pending([&inst]() -> HRESULT { return inst->Unregister(); }) & 0xFFFFFFFF);
            if (shared::ctx.state == shared::State::OFF) {
                return;
            }

            /// Register and activate
            logln("{}_register: {:#x}", log_prefix, com::checked(inst->Register(name, name, 0, 0)));
            logln("{}_update: {:#x}", log_prefix, com::checked(inst->UpdateStatus(WSCSecurityProductState::ON, static_cast<BOOL>(true))));

            /// Update the substatuses, if the interface supports this
            if constexpr (std::is_same_v<Ty, IWscAVStatus4>) {
                logln("{}_scan_update: {:#x}", log_prefix, com::checked(inst->UpdateScanSubstatus(WSCSecurityProductSubStatus::NO_ACTION)));
                logln("{}_settings_update: {:#x}", log_prefix, com::checked(inst->UpdateSettingsSubstatus(WSCSecurityProductSubStatus::NO_ACTION)));
                logln("{}_prot_update: {:#x}", log_prefix, com::checked(inst->UpdateProtectionUpdateSubstatus(WSCSecurityProductSubStatus::NO_ACTION)));
            }
        }
    } // namespace

    void startup() {
        /// Setup
        shared::ctx.deserialize();
        logln("init: {:#x}", com::checked(CoInitialize(nullptr)));

        /// WSC will reject the register request if name is empty
        auto name_w = std::wstring(shared::ctx.name.begin(), shared::ctx.name.end());
        if (name_w.empty()) {
            throw std::runtime_error("AV Name can not be empty!");
        }

        /// Convert to BSTR
        auto name = SysAllocString(name_w.c_str());
        defer->void {
            SysFreeString(name);
        };

        /// Register our stuff in the WSC interfaces
        apply<IWscASStatus>("IWscASStatus", name);
        apply<IWscAVStatus4>("IWscAVStatus4", name);
    }
} // namespace defendnot
