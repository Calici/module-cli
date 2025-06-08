module;
#include <format>
#include <rfl.hpp>
#include <string>
#include <tuple>
export module pharmaco.core.module_:run_data;
import moderna.generic;
import moderna.io;
import moderna.process;
import :dir;
import :runner;

namespace pharmaco::core {
  namespace generic = moderna::generic;
  namespace proc = moderna::process;
  namespace io = moderna::io;

  export enum struct run_mode { DEBUG, TEST, PRODUCTION };

  /*
    Encapsulates the run data required to run a module.
  */
  export class module_run_data {
    using run_error_creator = generic::staged_error_creator<
      generic::make_static_string("OPEN_STDOUT_FILE"),
      generic::make_static_string("OPEN_STDERR_FILE"),
      generic::make_static_string("OPEN_STDIN_PIPE"),
      generic::make_static_string("WRITE_PARAMS"),
      generic::make_static_string("RUN_PROCESS")>;
    auto __open_process_io() {
      return io::open_file<io::open_mode::write_truncate>(work_dir.stdout_path())
        .transform_error(run_error_creator{0})
        .and_then([&](auto &&out_file) {
          return io::open_file<io::open_mode::write_truncate>(work_dir.stderr_path())
            .transform_error(run_error_creator{1})
            .and_then([&](auto &&err_file) {
              return io::open_pipe()
                .transform_error(run_error_creator{2})
                .transform([&](auto &&pipe) {
                  return std::tuple{std::move(out_file), std::move(pipe), std::move(err_file)};
                });
            });
        });
    }

  public:
    module_dir work_dir;
    rfl::Generic::Object parameters;
    std::string image_name;
    run_mode mode = run_mode::DEBUG;
    std::optional<std::string> container_name;
    std::optional<std::string> shm_size;
    std::vector<std::pair<fs::path, fs::path>> volumes;
    std::optional<std::string> user;

    proc::subprocess_argument run_args() const {
      proc::subprocess_argument args{"docker", "run"};

      // Map the container name to use to run the module.
      if (container_name) {
        args.add_argument("--name", *container_name);
      }

      // Map the SHM Size required to run the module.
      if (shm_size) {
        args.add_argument("--shm_size", *shm_size);
      }
      // Gets the user used to run the module.
      if (user) {
        args.add_argument("--user", *user);
      }

      // This section describes the running mode for the modules.
      if (mode == run_mode::DEBUG) {
        args.add_argument("--env", "MODULE_RUN_MODE=DEBUG");
      } else if (mode == run_mode::TEST) {
        args.add_argument("--env", "MODULE_RUN_MODE=TEST");
      } else {
        args.add_argument("--env", "MODULE_RUN_MODE=PRODUCTION");
      }

      // This section describes the volume mapping for the modules.
      for (const auto &[host, cont] : volumes) {
        args.add_argument("-v", std::format("{}:{}", host.c_str(), cont.c_str()));
      }
      args.add_argument("-v", std::format("{}:{}", work_dir.run_path().c_str(), "/data"));

      // Finalizes and run the module with one positional argument, the parameters path
      args.add_argument(image_name, work_dir.params_path());
      return args;
    }

    std::expected<module_process, run_error_creator::error_type> run_module() {
      return __open_process_io().and_then([&](auto &&process_ios) {
        return work_dir.write_module_parameters(parameters)
          .transform_error(run_error_creator{3})
          .and_then([&]() {
            return proc::spawn(
                     run_args(),
                     io::get_native_handle(std::get<0>(process_ios)),
                     io::get_native_handle(std::get<1>(process_ios).reader),
                     io::get_native_handle(std::get<2>(process_ios))
            )
              .transform_error(run_error_creator{4})
              .transform([&](auto &&proc) {
                return module_process{std::move(proc), std::move(std::get<1>(process_ios).writer)};
              });
          });
      });
    }
  };
}