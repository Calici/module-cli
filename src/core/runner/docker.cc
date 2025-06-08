module;
#include <expected>
#include <filesystem>
#include <format>
#include <optional>
#include <string>
export module pharmaco.core.runner:docker;
import pharmaco.core.module_;
import moderna.generic;
import moderna.process;
import moderna.io;

namespace generic = moderna::generic;
namespace proc = moderna::process;
namespace fs = std::filesystem;
namespace io = moderna::io;
namespace pharmaco::core {
  export template <class data_type> class docker_process {
    using input_file = io::generic_file<io::file_type, false, true, false, true, false>;
    proc::subprocess __proc;
    data_type __data;
    input_file __stdin;

  public:
    docker_process(proc::subprocess proc, data_type data, input_file file) :
      __proc{std::move(proc)}, __data{std::move(data)}, __stdin{std::move(file)} {}
    proc::subprocess &process() {
      return __proc;
    }
    using stop_error_type = proc::subprocess_error;
    std::expected<proc::subprocess_result, stop_error_type> stop(
      std::chrono::milliseconds timeout = std::chrono::seconds{10}
    ) {
      using result_type = std::expected<proc::subprocess_result, stop_error_type>;
      return __proc.wait_non_blocking(false).and_then(
        [&](std::optional<proc::subprocess_result> &&res) {
          if (res) {
            return result_type{res.value()};
          } else {
            return __proc.send_signal(SIGTERM).and_then([&](auto &&) {
              return __proc.wait_or_kill(timeout, false);
            });
          }
        }
      );
    }

    std::expected<size_t, io::fs_error> write_to_stdin(std::string_view content) {
      return __stdin.write(content);
    }
  };
  using run_error_creator = generic::staged_error_creator<
    generic::make_static_string("OPEN_STDOUT_FILE"),
    generic::make_static_string("OPEN_STDERR_FILE"),
    generic::make_static_string("OPEN_STDIN_PIPE"),
    generic::make_static_string("RUN_PROCESS")>;

  struct runner_docker_data {
    std::string image_name;
    module_dir work_dir;
    std::vector<std::pair<fs::path, fs::path>> volume_mappings = {};
    std::vector<std::string> env_mappings = {};
    std::optional<std::string> shm_size = std::nullopt;
    std::optional<std::string> user = std::nullopt;
    std::optional<std::string> container_name = std::nullopt;
    bool force_pull = true;
    bool auto_delete = true;

    proc::subprocess_argument get_run_arguments() const {
      proc::subprocess_argument args{"docker", "run"};
      // Volume Mappings
      for (const auto &[host_path, target_path] : volume_mappings) {
        args.add_argument("-v", std::format("{}:{}", host_path.c_str(), target_path.c_str()));
      }
      args.add_argument("-v", std::format("{}:{}", work_dir.dir().c_str(), "/data"));
      // Env Entries
      for (const auto &env_entry : env_mappings) {
        args.add_argument("--env", env_entry);
      }
      if (container_name) {
        args.add_argument("--name", container_name.value());
      }
      if (force_pull) {
        args.add_argument("--pull", "always");
      }
      if (shm_size) {
        args.add_argument("--shm-size", shm_size.value());
      }
      if (user) {
        args.add_argument("--user", user.value());
      }
      if (auto_delete) {
        args.add_argument("--rm");
      }
      args.add_argument(image_name);
      args.add_argument("/data/params.yml");
      return args;
    }

    std::expected<docker_process<runner_docker_data>, run_error_creator::error_type> run() const {
      auto stdout_file = io::open_file<io::open_mode::write_truncate>(work_dir.stdout_path());
      if (!stdout_file) {
        return std::unexpected{run_error_creator{0}(stdout_file.error())};
      }
      auto stderr_file = io::open_file<io::open_mode::write_truncate>(work_dir.stderr_path());
      if (!stderr_file) {
        return std::unexpected{run_error_creator{1}(stderr_file.error())};
      }
      auto pipe_file = io::open_pipe();
      if (!pipe_file) {
        return std::unexpected{run_error_creator{2}(pipe_file.error())};
      }
      auto process = proc::spawn(
        get_run_arguments(),
        io::get_native_handle(stdout_file.value()),
        io::get_native_handle(pipe_file->reader),
        io::get_native_handle(stderr_file.value())
      );
      return docker_process{
        std::move(process.value()), std::move(*this), std::move(pipe_file->writer)
      };
    }
  };

}