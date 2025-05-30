#include "core/core.hpp"
#include "shared/strings.hpp"
#include "util/scm.hpp"

#include <format>
#include <print>
#include <stdexcept>

namespace loader {
    void ensure_environment() {
        auto manager = scm::Manager();
        if (!manager.valid()) [[unlikely]] {
            throw std::runtime_error("Unable to open scm::Manager");
        }

        auto service = manager.get_service(L"wscsvc");
        if (!service.valid() || !service.query_status()) {
            throw std::runtime_error(std::format("{}\nOpen error: {}", strings::wsc_unavailable_error().data(), GetLastError()));
        }

        if (service.state() == scm::ServiceState::RUNNING) {
            /// Wsc service has been already started, no need to start it ourselves
            return;
        }

        /// Let's start the service ourselves
        std::println("** wscsvc is not running, starting it..");
        if (!service.start()) {
            throw std::runtime_error(std::format("{}\nTried to start the service, but go an error: {}", strings::wsc_unavailable_error(), GetLastError()));
        }

        std::println("** successfully started the service, waiting for it to get up..");
        while (true) {
            if (!service.query_status(/*force=*/true)) {
                throw std::runtime_error(
                    std::format("{}\nStarted the service, got an error while querying: {}", strings::wsc_unavailable_error(), GetLastError()));
            }

            if (const auto state = service.state(); state == scm::ServiceState::RUNNING) {
                std::println("** we are good to go");
                return;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
} // namespace loader
