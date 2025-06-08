module;
#include <rfl/yaml.hpp>
#include <chrono>
#include <csignal>
#include <expected>
#include <optional>
#include <rfl.hpp>
#include <thread>
export module pharmaco.core.module_:runner;
import moderna.process;
import moderna.generic;
import moderna.io;

namespace pharmaco::core {
  namespace proc = moderna::process;
  namespace generic = moderna::generic;
  namespace chrono = std::chrono;
  namespace io = moderna::io;

  export class module_process {
    using pipe_writer = decltype(io::open_pipe()->writer);
    chrono::milliseconds __stop_timeout;
    proc::subprocess __process;
    pipe_writer __stdin_writer;

  public:
    module_process(
      proc::subprocess process,
      pipe_writer stdin_writer,
      chrono::milliseconds stop_timeout = chrono::seconds{10}
    ) :
      __process{std::move(process)}, __stdin_writer{std::move(stdin_writer)},
      __stop_timeout{stop_timeout} {}

    proc::subprocess &process() {
      return __process;
    }

    template <class... Args> auto communicate(Args &&...args) {
      return __stdin_writer.write(std::forward<Args>(args)...);
    }

    using stop_error_creator = generic::staged_error_creator<
      generic::make_static_string("WAIT_NON_BLOCK"),
      generic::make_static_string("SIGTERM"),
      generic::make_static_string("WAIT_TIMEOUT_NON_BLOCK"),
      generic::make_static_string("KILL_WAIT")>;
    using stop_error_type = typename stop_error_creator::error_type;
    std::expected<proc::subprocess_result, stop_error_type> stop() {
      using return_type = std::expected<proc::subprocess_result, stop_error_type>;
      return __process.wait_non_blocking(false)
        .transform_error(stop_error_creator{0})
        .and_then([&](std::optional<proc::subprocess_result> &&opt_res) {
          if (opt_res) {
            return return_type{*opt_res};
          } else {
            return __process.send_signal(SIGTERM)
              .transform_error(stop_error_creator{1})
              .and_then([&](proc::subprocess &proc) {
                std::this_thread::sleep_for(__stop_timeout);
                return proc.wait_non_blocking(false).transform_error(stop_error_creator{2});
              })
              .and_then([&](std::optional<proc::subprocess_result> &&opt_res) {
                if (opt_res) {
                  return return_type{*opt_res};
                } else {
                  return __process.kill_and_wait(false).transform_error(stop_error_creator{3});
                }
              });
          }
        });
    }

    void on_exit() {}
  };
}