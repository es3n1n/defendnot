#include "core/core.hpp"
#include "shared/ctx.hpp"
#include "shared/defer.hpp"
#include "shared/ipc.hpp"
#include "shared/names.hpp"
#include <argparse/argparse.hpp>

#include <format>
#include <print>
#include <thread>

namespace {
    void setup_window(const loader::Config& config) {
        if ((!config.from_autorun || config.verbose) && config.alloc_console) {
            shared::alloc_console();
        }
    }

    void setup_context(const loader::Config& config) {
        std::println("** setting up context");

        if (config.name.length() > shared::kMaxNameLength) {
            throw std::runtime_error(std::format("Max name length is {} characters", shared::kMaxNameLength));
        }

        shared::ctx.state = config.disable ? shared::State::OFF : shared::State::ON;
        shared::ctx.verbose = config.verbose;
        std::ranges::copy(config.name, shared::ctx.name.data());

        /// No need to overwrite ctx if we are called from autorun
        if (!config.from_autorun) {
            std::println("** overwriting ctx.bin");
            shared::ctx.serialize();
        }
    }

    [[nodiscard]] HANDLE load_defendnot() {
        std::println("** loading defendnot");

        auto dll_path = shared::get_this_module_path().parent_path();
        dll_path /= names::kDllName;
        if (!std::filesystem::exists(dll_path)) {
            throw std::runtime_error(std::format("{} does not exist!", names::kDllName));
        }

        return loader::inject(dll_path.string(), names::kVictimProcess);
    }

    void wait_for_finish(shared::InterProcessCommunication& ipc) {
        std::println("** waiting for process to finish, this can take a while");
        std::cout << std::flush;
        while (!ipc->finished) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        std::println("** success: {}", ipc->success);
    }

    void process_autorun(const loader::Config& config) {
        if (shared::ctx.state == shared::State::ON && config.enable_autorun) {
            std::println("** added to autorun: {}", loader::add_to_autorun(config.autorun_type));
        } else {
            std::println("** removed from autorun: {}", loader::remove_from_autorun());
        }
    }

    void banner(const loader::Config& config) {
        std::println();
        std::println("thanks for using {}", names::kProjectName);
        std::println("please don't forget to leave a star at {}", names::kRepoUrl);

        if (!config.from_autorun && config.alloc_console) {
            system("pause");
        }
    }
} // namespace

int main(int argc, char* argv[]) try {
    argparse::ArgumentParser program(std::format("{}-loader", names::kProjectName), names::kVersion.data(), argparse::default_arguments::none);

    const auto fatal_print = [](const std::string_view str) -> void {
        shared::alloc_console();
        std::cerr << str << std::endl;
        system("pause");
        std::exit(EXIT_FAILURE);
    };

    /// We are registering these ourselves because we have to alloc console first
    program.add_argument("-h", "--help")
        .help("prints help message and exits")
        .default_value(false)
        .implicit_value(true)
        .action([&fatal_print, &program](const auto& /*unused*/) -> void { fatal_print(program.help().str()); });
    program.add_argument("--version")
        .help("shows version and exits")
        .default_value(false)
        .implicit_value(true)
        .action([&fatal_print](const auto& /*unused*/) -> void { fatal_print(std::format("{}-loader v{}", names::kProjectName, names::kVersion)); });

    /// defendnot-loader parameters:
    program.add_argument("-n", "--name").help("av display name").default_value(std::string(names::kDefaultAVName)).nargs(1);
    program.add_argument("-d", "--disable").help(std::format("disable {}", names::kProjectName)).default_value(false).implicit_value(true);
    program.add_argument("-v", "--verbose").help("verbose logging").default_value(false).implicit_value(true);
    program.add_argument("--silent").help("do not allocate console").default_value(false).implicit_value(true);
    program.add_argument("--autorun-as-user").help("create autorun task as currently logged in user").default_value(false).implicit_value(true);
    program.add_argument("--disable-autorun").help("disable autorun task creation").default_value(false).implicit_value(true);
    program.add_argument("--from-autorun").hidden().default_value(false).implicit_value(true);

    try {
        program.parse_args(argc, argv);
    } catch (std::exception& e) {
        std::stringstream ss;
        ss << e.what() << '\n';
        ss << program.help().str();
        fatal_print(ss.str());
        return EXIT_FAILURE;
    }

    auto config = loader::Config{
        .name = program.get<std::string>("-n"),
        .disable = program.get<bool>("-d"),
        .alloc_console = !program.get<bool>("--silent"),
        .verbose = program.get<bool>("-v"),
        .from_autorun = program.get<bool>("--from-autorun"),
        .autorun_type = program.get<bool>("--autorun-as-user") ? /// As system on boot is the default value
                            loader::AutorunType::AS_CURRENT_USER_ON_LOGIN :
                            loader::AutorunType::AS_SYSTEM_ON_BOOT,
        .enable_autorun = !program.get<bool>("--disable-autorun"),
    };

    if (!config.alloc_console && config.verbose) {
        fatal_print("--silent flag can not be used in combination with --verbose");
    }

    setup_window(config);
    setup_context(config);

    /// \todo @es3n1n: move this to a separate function and add move ctor for ipc
    std::println("** setting up ipc");
    auto ipc = shared::InterProcessCommunication(shared::InterProcessCommunicationMode::READ_WRITE, true);
    ipc->finished = false;

    const auto process = load_defendnot();
    defer->void {
        TerminateProcess(process, 0);
    };

    wait_for_finish(ipc);
    process_autorun(config);
    banner(config);

    return EXIT_SUCCESS;
} catch (std::exception& err) {
    std::println(stderr, "** fatal error: {}", err.what());
    system("pause");
    return EXIT_FAILURE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    return main(__argc, __argv);
}
