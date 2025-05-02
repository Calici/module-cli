module;
#include <optional>
#include <string_view>
export module pharmaco.cli:build_action;
import moderna.cli;
import moderna.process;
import :config;

namespace pharmaco::cli {
  namespace cli = moderna::cli;
  namespace proc = moderna::process;
  export struct build_action {
    std::optional<cli::app_version> get_version(cli::app &app) const {
      auto version_str = app.arguments().first_of("--version");
      if (version_str) {
        auto version = cli::app_version::from_string(version_str.value());
        if (!version) {
          app.error(1, "Invalid version string : {}", version_str.value());
        }
        return version.value();
      }
      return std::nullopt;
    }
    std::string get_container_name(
      cli::app &app, const module_config_file &file, const std::optional<cli::app_version> &version
    ) const {
      if (version) {
        auto container_name = file.container_name(version.value());
        if (!container_name) {
          app.error(1, "Module {} does not have version : {}", file.name(), version.value());
        }
        return container_name.value();
      } else {
        return file.container_name();
      }
    }
    void operator()(cli::app &app) {
      // Argument Parsing
      app.add_argument("--version")
        .optional()
        .help("Version to build for, otherwise this will default to the newest possible version");
      app.parse_argument();

      // Processing
      auto file = app.test_expected(module_config_file::load(), 1, "Error opening file: {}");
      auto version = get_version(app);
      auto container_name = get_container_name(app, file, version);

      // Processing Again
      app.out("Building for module \"{}\" with tag \"{}\"", file.name(), container_name);
      auto res = proc::subprocess::run(
        proc::make_argument(
          "docker",
          "build",
          "-f",
          file.config().container.dockerfile,
          "-t",
          container_name,
          file.config().container.build.context
        )
      );
      if (not res) {
        app.error(1, "Fail to run docker: {}", res.error().what());
      }
      app.out("Built container: {}", container_name);
    }
  };
}