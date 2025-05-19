module;
#include <algorithm>
#include <filesystem>
#include <format>
#include <ranges>
export module pharmaco.cli:init_action;
import moderna.cli;
import moderna.process;
import :config;

namespace proc = moderna::process;
namespace fs = std::filesystem;
namespace pharmaco::cli {
  namespace cli = moderna::cli;
  export struct init_action {
    fs::path assets_dir;

    std::string make_internal_name(std::string_view name) {
      std::string internal_name{name};
      std::ranges::replace(internal_name, ' ', '-');
      return internal_name;
    }

    void validate_internal_name(cli::app &app, std::string_view internal_name) {
      if (internal_name.find(' ') != std::string::npos) {
        app.error(1, "{} contains spaces", internal_name);
      } else if (internal_name.find('\t') != std::string::npos) {
        app.error(1, "{} contains tabs", internal_name);
      }
    }

    void validate_directory(cli::app &app, fs::path &dir) {
      if (fs::exists(dir) && not fs::is_directory(dir)) {
        app.error(1, "Error: {} is not a directory", dir.c_str());
      } else if (fs::exists(dir) && not fs::is_empty(dir)) {
        app.error(1, "Error: {} is not empty", dir.c_str());
      } else if (not fs::exists(dir)) {
        fs::create_directories(dir);
        dir = fs::canonical(dir);
      }
    }

    void operator()(cli::app &app) {
      app.add_argument("--name").required().help("The name of the module");
      app.add_argument("--developer").required().help("The name of the developer of the module");
      app.add_argument("--internal_name")
        .optional()
        .help(
          "The internal name of the module. If not given, it will be inferred from the name of the module"
        );
      app.add_argument("--path").optional().help(
        "The path to the directory to initialize the module on"
      );
      app.parse_argument();

      auto name = app.arguments().first_of("--name").value();
      auto internal_name = app.arguments()
                             .first_of("--internal_name")
                             .transform([&](std::string_view n) {
                               validate_internal_name(app, n);
                               return std::string{n};
                             })
                             .value_or(make_internal_name(name));
      auto developer = app.arguments().first_of("--developer").value();
      /*
        Validate internal name if exists
      */
      fs::path dir =
        app.arguments().first_of("--path").transform([](std::string_view v) { return fs::path{v}; }
        ).value_or(fs::current_path());
      validate_directory(app, dir);

      app.out("Created a Module Directory at {}", dir.c_str());
      /*
        Initialize module directory by putting a configuration file.
      */
      auto config = app.test_expected(
        module_config_file::new_file(
          dir / "config.yml", std::string{name}, internal_name, std::string{developer}
        ),
        1,
        "Error creating file : {}"
      );
      /*
        Create directories and add placeholders based on the configuration file
      */
      initialize_directory(dir, config);
      // run_program("git init {}", dir.c_str());
      auto res = proc::subprocess::run(proc::subprocess_argument("git", "init", dir.string()));
      if (!res) {
        app.error(1, "Failed to initialize git with error : {}", res.error().what());
      }
    }

    void initialize_directory(const fs::path &dir, const module_config_file &config) {
      // Write module icons
      std::filesystem::create_directories(config.icon().parent_path());
      std::filesystem::copy(assets_dir / "default_dir" / "module-default-icon.png", config.icon());
      std::filesystem::create_directories(config.dark_icon().parent_path());
      std::filesystem::copy(
        assets_dir / "default_dir" / "module-default-icon.png", config.dark_icon()
      );
      /*
        Write manifest out
      */
      std::filesystem::create_directories(config.manifest_file().parent_path());
      std::filesystem::copy(assets_dir / "default_dir" / "manifest.py", config.manifest_file());

      // Write Documentation Folder
      std::filesystem::create_directories(config.docs().parent_path());
      std::filesystem::copy(
        assets_dir / "default_dir" / "default-docs", config.docs(), fs::copy_options::recursive
      );
      // Write dockerfile
      std::filesystem::copy(assets_dir / "default_dir" / "dockerfile", config.dockerfile());
      // WRite src
      std::filesystem::create_directories(config.dir() / "src");
    }
  };
};