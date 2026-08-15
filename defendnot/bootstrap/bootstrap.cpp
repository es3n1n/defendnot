#include "bootstrap.hpp"
#include "core/com.hpp"
#include "core/log.hpp"
#include "shared/ctx.hpp"
#include "shared/defer.hpp"

#include <Windows.h>

namespace defendnot {
    namespace {
        void activate(const auto& step, IWscASStatus* inst) {
            step("update", inst->UpdateStatus(WSCSecurityProductState::ON, TRUE));
        }

        void activate(const auto& step, IWscAVStatus4* inst) {
            step("update", inst->UpdateStatus(WSCSecurityProductState::ON, TRUE));
            step("scan_update", inst->UpdateScanSubstatus(WSCSecurityProductSubStatus::NO_ACTION));
            step("settings_update", inst->UpdateSettingsSubstatus(WSCSecurityProductSubStatus::NO_ACTION));
            step("prot_update", inst->UpdateProtectionUpdateSubstatus(WSCSecurityProductSubStatus::NO_ACTION));
        }

        void activate(const auto& step, IWscFWStatus2* inst) {
            step("update", inst->UpdateStatus(WSCSecurityProductState::ON));
            step("domain_update", inst->UpdateDomainProfileSubstatus(WSCSecurityProductSubStatus::NO_ACTION));
            step("private_update", inst->UpdatePrivateProfileSubstatus(WSCSecurityProductSubStatus::NO_ACTION));
            step("public_update", inst->UpdatePublicProfileSubstatus(WSCSecurityProductSubStatus::NO_ACTION));
        }

        template <com::ComObject Ty>
        void apply(const std::string_view log_prefix, const BSTR name, const bool should_register) {
            /// Get the WSC interface
            auto inst = com::query<Ty>();

            /// This can fail if we dont have any products registered so no com::checked
            logln("{}_unregister: {:#x}", log_prefix, com::retry_while_pending([&inst]() -> HRESULT { return inst->Unregister(); }) & 0xFFFFFFFF);
            if (!should_register) {
                return;
            }

            /// Register and activate
            const auto step = [&](const std::string_view what, const HRESULT hr) -> void {
                logln("{}_{}: {:#x}", log_prefix, what, com::checked(hr));
            };
            step("register", inst->Register(name, name, 0, 0));
            activate(step, inst.get());
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
        defer {
            SysFreeString(name);
        };

        /// Register our stuff in the WSC interfaces
        const bool enabled = shared::ctx.state != shared::State::OFF;
        apply<IWscASStatus>("IWscASStatus", name, enabled);
        apply<IWscAVStatus4>("IWscAVStatus4", name, enabled);
        apply<IWscFWStatus2>("IWscFWStatus2", name, enabled && shared::ctx.register_firewall);
    }
} // namespace defendnot
