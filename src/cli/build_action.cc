module;
#include <filesystem>
#include <format>
#include <optional>
#include <string_view>
export module pharmaco.cli:build_action;
import moderna.cli;
import moderna.process;
import :loader;

namespace pharmaco::cli {
  namespace cli = moderna::cli;
  namespace proc = moderna::process;
  namespace fs = std::filesystem;
  export struct build_action {
    std::optional<cli::app_version> get_version(cli::app &app) const {
      auto version_str = app.arguments().first_of("--version");
      if (version_str) {
        auto version = cli::app_version::from_string(version_str.value());
        if (!version) {
          cli::app::error(1, "Invalid version string : {}", version_str.value());
        }
        return version.value();
      }
      return std::nullopt;
    }
    std::string get_container_name(
      const config_module &config, const std::optional<cli::app_version> &version
    ) const {
      if (version) {
        auto container_name = config.get_image_name(version.value());
        if (!container_name) {
          cli::app::warn(
            "module {} does not have version : {}", config.identity.name, version.value()
          );
          cli::app::warn("available are:");
          for (const auto &version : config.versions) {
            cli::app::warn("- {}", version.version);
          }
          cli::app::error(1, "");
        }
        return container_name.value();
      } else {
        return config.get_image_name();
      }
    }
    auto run_build(
      std::string_view container_name, const config_module &config, const fs::path &module_dir
    ) {
      auto args = config.get_build_arguments(container_name, module_dir);
      return cli::app::test_expected(proc::subprocess::run(args));
    }
    void operator()(cli::app &app) {
      // Argument Parsing
      app.add_argument("--version")
        .optional()
        .help("Version to build for, otherwise this will default to the newest possible version");
      app.parse_argument();

      // Processing
      auto file = load_config_module();
      auto version = get_version(app);
      auto container_name = get_container_name(file, version);

      // Processing Again
      cli::app::out(
        "building for module \"{}\" with tag \"{}\"", file.identity.name, container_name
      );
      run_build(container_name, file, get_default_module_dir());
      cli::app::out("built container: {}", container_name);
    }
  };
}