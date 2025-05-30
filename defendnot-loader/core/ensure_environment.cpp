#include "core/core.hpp"
#include "shared/strings.hpp"
#include "util/scm.hpp"

#include <format>
#include <stdexcept>

namespace loader {
    void ensure_environment() {
        auto manager = scm::Manager();
        if (!manager.valid()) [[unlikely]] {
            throw std::runtime_error("Unable to open scm::Manager");
        }

        auto service = manager.get_service(L"wscsvc");
        if (!service.valid() || !service.query_status()) {
            throw std::runtime_error(strings::wsc_unavailable_error().data());
        }

        const auto state = service.state();
        if (state != scm::ServiceState::RUNNING) {
            throw std::runtime_error(std::format("{}\nService state: {}, but should be RUNNING", strings::wsc_unavailable_error(),
                                                 scm::kServiceStateNames[static_cast<std::underlying_type_t<scm::ServiceState>>(state)]));
        }
    }
} // namespace loader
