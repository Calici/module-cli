module;
#include <filesystem>
#include <format>
#include <optional>
#include <string>
#include <vector>
export module pharmaco.cli:config_container;
import moderna.process;

namespace fs = std::filesystem;
namespace proc = moderna::process;
namespace pharmaco::cli {

  struct config_container_build {
    fs::path context;
    std::vector<std::string> build_args;
    std::optional<std::string> target;

    fs::path get_context_path(const fs::path &work_dir) const {
      return work_dir / context;
    }
  };

  struct config_container_run {
    std::optional<std::string> shm_size;
    std::vector<std::string> env_vars;
  };

  struct config_container {
    config_container_build build_config;
    config_container_run run_config;

    fs::path get_dockerfile_path(const fs::path &work_dir) const {
      return work_dir / "dockerfile";
    }

    proc::subprocess_argument get_build_arguments(
      std::string_view container_name, const fs::path &work_dir
    ) const {
      proc::subprocess_argument args{
        "docker", "build", "-f", get_dockerfile_path(work_dir).string()
      };
      for (const auto &build_arg : build_config.build_args) {
        args.add_argument("--build-arg", build_arg);
      }
      if (build_config.target) {
        args.add_argument("--target", build_config.target.value());
      }
      args.add_argument("-t", container_name);
      args.add_argument(build_config.get_context_path(work_dir).string());
      return args;
    }
    proc::subprocess_argument get_run_arguments(
      std::string_view container_name,
      const fs::path &params_file,
      std::optional<std::string_view> run_user = std::nullopt,
      const std::vector<std::pair<fs::path, fs::path>> &volume_maps = {},
      const std::vector<std::string> &additional_envs = {},
      bool auto_pull = true
    ) const {
      proc::subprocess_argument args{"docker", "run"};
      if (auto_pull) {
        args.add_argument("--pull");
      }
      for (const auto &[host_path, docker_path] : volume_maps) {
        args.add_argument("-v", std::format("{}:{}", host_path.c_str(), docker_path.c_str()));
      }
      for (const auto &env : additional_envs) {
        args.add_argument("--env", env);
      }
      for (const auto &env : run_config.env_vars) {
        args.add_argument("--env", env);
      }
      if (run_config.shm_size) {
        args.add_argument("--shm-size", run_config.shm_size.value());
      }
      if (run_user) {
        args.add_argument("--user", run_user.value());
      }
      args.add_argument(container_name);
      args.add_argument(params_file.c_str());
    }

    static config_container make_default() {
      return config_container{
        .build_config{.context{"."}, .build_args{}, .target{std::nullopt}},
        .run_config{.shm_size{std::nullopt}, .env_vars{}}
      };
    }
  };
}