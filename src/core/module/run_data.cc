module;
#include <rfl.hpp>
#include <string>
#include <tuple>
#include <string_view>
export module pharmaco.core:module_run_data;
import moderna.generic;
import moderna.io;
import moderna.process;
import :module_dir;
import :module_runner;

namespace pharmaco::core {
  namespace generic = moderna::generic;
  namespace proc = moderna::process;
  namespace io = moderna::io;
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
    std::string container_name;

    proc::subprocess_argument run_args() const {
      proc::subprocess_argument args{
        "docker",
        "run",
        "-v",
        std::format("{}:/data", work_dir.run_path().c_str()),
        container_name,
        "/data/params.yml"
      };
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