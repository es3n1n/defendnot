#include "core/core.hpp"

#include "shared/com.hpp"
#include "shared/ctx.hpp"
#include "shared/strings.hpp"

#include <memory>
#include <print>
#include <stdexcept>
#include <type_traits>

#include <comdef.h>
#include <taskschd.h>

#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "ole32.lib")

namespace loader {
    namespace {
        constexpr std::string_view kTaskName = strings::kProjectName;

        void co_initialize() {
            static std::once_flag fl;
            std::call_once(fl, []() -> void {
                const auto result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

                if (FAILED(result)) {
                    throw std::runtime_error("failed to CoInitializeEx");
                }
            });
        }

        template <typename Callable>
        [[nodiscard]] bool with_service(Callable&& callback) {
            co_initialize();

            com::Ptr<ITaskService> service;
            auto hr =
                CoCreateInstance(CLSID_TaskScheduler, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskService, reinterpret_cast<void**>(service.ref_to_ptr()));
            if (FAILED(hr)) {
                return false;
            }

            hr = service->Connect(variant_t{}, variant_t{}, variant_t{}, variant_t{});
            if (FAILED(hr)) {
                return false;
            }

            com::Ptr<ITaskFolder> root_folder;
            hr = service->GetFolder(bstr_t(L"\\"), root_folder.ref_to_ptr());
            if (FAILED(hr)) {
                return false;
            }

            /// Cleanup our task, we will recreate it in the callback if needed
            root_folder->DeleteTask(bstr_t(kTaskName.data()), 0);
            return callback(service.get(), root_folder.get());
        }
    } // namespace

    [[nodiscard]] bool add_to_autorun(AutorunType type) {
        const auto bin_path = shared::get_this_module_path();

        return with_service([bin_path, type](ITaskService* service, ITaskFolder* folder) -> bool {
            /// Deduce the autorun config based on type
            const auto task_trigger = type == AutorunType::AS_SYSTEM_ON_BOOT ? TASK_TRIGGER_BOOT : TASK_TRIGGER_LOGON;
            const auto logon_type = type == AutorunType::AS_SYSTEM_ON_BOOT ? TASK_LOGON_SERVICE_ACCOUNT : TASK_LOGON_INTERACTIVE_TOKEN;

            BSTR user_id = nullptr;
            auto bstr_sys = bstr_t(L"SYSTEM");
            if (type == AutorunType::AS_SYSTEM_ON_BOOT) {
                user_id = bstr_sys;
            }

            com::Ptr<ITaskDefinition> task;
            auto hr = service->NewTask(0, task.ref_to_ptr());
            if (FAILED(hr)) {
                return false;
            }

            com::Ptr<IRegistrationInfo> reg_info;
            hr = task->get_RegistrationInfo(reg_info.ref_to_ptr());
            if (FAILED(hr)) {
                return false;
            }

            com::Ptr<IPrincipal> principal;
            hr = task->get_Principal(principal.ref_to_ptr());
            if (FAILED(hr)) {
                return false;
            }

            com::Ptr<ITriggerCollection> trigger_collection;
            hr = task->get_Triggers(trigger_collection.ref_to_ptr());
            if (FAILED(hr)) {
                return false;
            }

            com::Ptr<ITrigger> trigger;
            hr = trigger_collection->Create(task_trigger, trigger.ref_to_ptr());
            if (FAILED(hr)) {
                return false;
            }

            com::Ptr<IActionCollection> action_collection;
            hr = task->get_Actions(action_collection.ref_to_ptr());
            if (FAILED(hr)) {
                return false;
            }

            com::Ptr<IAction> action;
            hr = action_collection->Create(TASK_ACTION_EXEC, action.ref_to_ptr());
            if (FAILED(hr)) {
                return false;
            }

            com::Ptr<IExecAction> exec_action;
            hr = action->QueryInterface(IID_IExecAction, reinterpret_cast<void**>(exec_action.ref_to_ptr()));
            if (FAILED(hr)) {
                return false;
            }

            com::Ptr<ITaskSettings> settings;
            hr = task->get_Settings(settings.ref_to_ptr());
            if (FAILED(hr)) {
                return false;
            }

            /// Elevated, when system boots
            principal->put_UserId(user_id);
            principal->put_LogonType(logon_type);
            principal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);

            /// Info
            reg_info->put_Author(bstr_t(strings::kRepoUrl.data()));

            /// Start even if we're on batteries
            settings->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
            settings->put_StopIfGoingOnBatteries(VARIANT_FALSE);

            /// Binary
            exec_action->put_Path(bstr_t(bin_path.string().c_str()));
            exec_action->put_Arguments(bstr_t("--from-autorun"));

            /// Register the task and we are done
            com::Ptr<IRegisteredTask> registered_task;
            hr = folder->RegisterTaskDefinition(bstr_t(kTaskName.data()), task.get(), TASK_CREATE_OR_UPDATE, VARIANT{}, VARIANT{}, TASK_LOGON_NONE,
                                                variant_t(L""), registered_task.ref_to_ptr());
            return SUCCEEDED(hr);
        });
    }

    [[nodiscard]] bool remove_from_autorun() {
        return with_service([]<typename... TArgs>(TArgs...) -> bool { return true; });
    }
} // namespace loader
