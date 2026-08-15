#pragma once
#include <array>
#include <cstdint>
#include <cstdio>
#include <fstream>

#include "shared/util.hpp"

#pragma pack(push, 1)
namespace shared {
    constexpr std::size_t kMaxNameLength = 128;
    constexpr std::string_view kCtxPath = "ctx.bin";

    namespace detail {
        inline std::string ctx_path() {
            auto path = get_this_module_path().parent_path();
            path /= kCtxPath;
            return path.string();
        }
    } // namespace detail

    enum class State : std::uint8_t {
        OFF = 0,
        ON,
    };

    inline constinit struct Context {
    public:
        State state = State::ON;
        bool verbose = false;
        std::array<char, kMaxNameLength + 1> name = {0}; // +1 for the nullterm
        bool register_firewall = false;

        void serialize() const {
            std::ofstream stream(detail::ctx_path(), std::ios::binary);
            if (!stream.good()) {
                throw std::runtime_error("can not write ctx.bin");
            }

            stream.write(reinterpret_cast<const char*>(this), sizeof(*this));
        }

        void deserialize() {
            std::ifstream stream(detail::ctx_path(), std::ios::binary);
            if (!stream.good()) {
                throw std::runtime_error("can not read ctx.bin");
            }

            stream.read(reinterpret_cast<char*>(this), sizeof(*this));
        }
    } ctx = {};

    static_assert(std::is_trivially_copyable_v<Context>);
} // namespace shared
#pragma pack(pop)
