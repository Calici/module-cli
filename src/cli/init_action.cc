module;
#include <rfl/yaml.hpp>
#include <algorithm>
#include <filesystem>
#include <format>
#include <string>
#include <string_view>
export module pharmaco.cli:init_action;
import moderna.cli;
import moderna.io;
import moderna.process;
import :config_module;

namespace proc = moderna::process;
namespace io = moderna::io;
namespace fs = std::filesystem;
namespace pharmaco::cli {
  namespace cli = moderna::cli;
  export struct init_action {
    fs::path assets_dir;

    std::string name_to_internal_name(std::string_view name, char replace_character = '_') {
      std::string internal_name{name};
      std::ranges::replace(internal_name, ' ', replace_character);
      return internal_name;
    }

    void check_internal_name(cli::app &app, std::string_view internal_name) {
      if (internal_name.find(' ') != std::string::npos) {
        cli::app::error(1, "error: {} contains spaces", internal_name);
      } else if (internal_name.find('\t') != std::string::npos) {
        cli::app::error(1, "error: {} contains tabs", internal_name);
      }
    }

    void check_directory(cli::app &app, fs::path &work_dir) {
      if (fs::exists(work_dir) && not fs::is_directory(work_dir)) {
        cli::app::error(1, "error: {} is not a directory", work_dir.c_str());
      } else if (fs::exists(work_dir) && not fs::is_empty(work_dir)) {
        cli::app::error(1, "error: {} is not an empty directory", work_dir.c_str());
      } else if (not fs::exists(work_dir)) {
        fs::create_directories(work_dir);
        work_dir = fs::absolute(work_dir);
      }
    }

    config_module create_config_file(
      const fs::path &work_dir,
      std::string_view name,
      std::string_view internal_name,
      std::string_view developer
    ) {
      auto config = config_module::make_default(
        std::string{name}, std::string{internal_name}, std::string{developer}
      );
      cli::app::test_expected(rfl::yaml::save((work_dir / "config.yml").c_str(), config));
      return config;
    }

    void initialize_directory(const fs::path &work_dir, const config_module &config) {
      fs::path icon_path = config.get_icon_path(work_dir);
      fs::path dark_icon_path = config.get_dark_icon_path(work_dir);
      fs::path docs_dir = config.get_docs_directory(work_dir);
      // Write module icons
      std::filesystem::create_directories(icon_path.parent_path());
      std::filesystem::copy(assets_dir / "default_dir" / "module-default-icon.png", icon_path);
      std::filesystem::create_directories(dark_icon_path.parent_path());
      std::filesystem::copy(assets_dir / "default_dir" / "module-default-icon.png", dark_icon_path);
      /*
        Write manifest out
      */
      std::filesystem::copy(assets_dir / "default_dir" / "manifest.py", work_dir / "manifest.py");

      // Write Documentation Folder
      std::filesystem::copy(
        assets_dir / "default_dir" / "docs", docs_dir, fs::copy_options::recursive
      );
      // Write dockerfile
      std::filesystem::copy(
        assets_dir / "default_dir" / "dockerfile", config.get_dockerfile_path(work_dir)
      );
      // WRite src
      std::filesystem::create_directories(work_dir / "src");
    }

    void initialize_git_repository(cli::app &app, const fs::path &work_dir) {
      auto null_file =
        cli::app::test_expected(io::open_file<io::open_mode::read_write_truncate>("/dev/null"));
      cli::app::test_expected(
        proc::subprocess::run(
          proc::subprocess_argument{"git", "init", work_dir.c_str()},
          true,
          io::get_native_handle(null_file),
          io::get_native_handle(null_file)
        )
      );
    }

    void operator()(cli::app &app) {
      app.add_argument("--name").required().help("name to give the module created");
      app.add_argument("--developer").required().help("name of the developer of the module");
      app.add_argument("--internal_name")
        .optional()
        .help("internal_name for the module, if this is not given, this will be inferred");
      app.add_argument("--path").optional().help(
        "the path to initialize the module on, if not given, the current work directory will be used"
      );
      app.parse_argument();

      auto name = app.arguments().first_of("--name").value();
      auto internal_name = app.arguments()
                             .first_of("--internal_name")
                             .transform([&](cli::argument_value &&arg) {
                               check_internal_name(app, arg.raw_value);
                               return std::string{arg.raw_value};
                             })
                             .value_or(name_to_internal_name(name));
      auto developer = app.arguments().first_of("--developer").value();
      /*
        Validate internal name if exists
      */
      fs::path work_dir =
        app.arguments()
          .first_of("--path")
          .transform([](cli::argument_value &&arg) { return arg.cast<fs::path>().value(); })
          .value_or(fs::current_path());
      check_directory(app, work_dir);

      cli::app::out("[√] created a module directory at {}", work_dir.c_str());
      auto config = create_config_file(work_dir, name, internal_name, developer);
      cli::app::out("[√] create a module config file");
      initialize_directory(work_dir, config);
      cli::app::out("[√] copied default files");
      initialize_git_repository(app, work_dir);
      cli::app::out("[√] create git repository");
    }
  };
};