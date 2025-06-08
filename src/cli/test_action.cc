module;
#include <filesystem>
#include <format>
export module pharmaco.cli:test_action;
import moderna.cli;
import :loader;

namespace pharmaco::cli {
  namespace cli = moderna::cli;
  namespace fs = std::filesystem;
  struct test_add_action {
    void operator()(cli::app &app) {
      app.add_argument("--name").help("The new test name").required();
      app.add_argument("--data").help("The path to the test data").required();
      app.parse_argument();

      auto config = load_config_module();
      auto name = app.arguments().first_of("--name").value();
      auto run_yml = app.arguments().first_of("--run_yml").value().cast<fs::path>().value();
      auto cur_test = config.get_test(name);
      if (cur_test) {
        cli::app::error(
          1, "test {} exists at {}", cur_test->get().name, cur_test->get().run_yml.c_str()
        );
      }
      auto &added_test = config.set_test({.name{name}, .run_yml{run_yml}});
      save_config_module(config);
      cli::app::out("test {} has been added", added_test.name);
    }
  };
  struct test_list_action {
    void operator()(cli::app &app) {
      auto config = load_config_module();
      cli::app::out("Available Tests :");
      for (const auto &test_config : config.tests) {
        cli::app::out("- {:16} at {}", test_config.name, test_config.run_yml.c_str());
      }
    }
  };
  struct test_run_action {
    void operator()(cli::app &app) {
      app.add_argument("--name").optional().help("The name of the test to run").multiple();
      app.add_argument("--parallel")
        .optional()
        .help("If the test should be run in parallel")
        .as_flag();
      app.add_argument("--strict")
        .optional()
        .help("Exit with an error if a specific test specified in name cannot be found")
        .as_flag();
      app.parse_argument();

      bool strict = app.arguments().contains("--strict");
      auto file = load_config_module();
      if (app.arguments().count("--name") == 0) {
        for (const auto &test : file.tests) {
          run_test(test, file);
        }
      } else {
        if (strict) {
          validate_test_names(file, app);
        }
        for (const auto &test_name_arg : app.arguments().filter("--name")) {
          auto test_config = file.get_test(test_name_arg);
          if (test_config) {
            run_test(*test_config, file);
          } else {
            cli::app::warn("warn: {} cannot be found", test_name_arg);
          }
        }
      }
    }
    void validate_test_names(const config_module &config, const cli::app &app) {
      for (const auto &test_name_arg : app.arguments().filter("--name")) {
        auto test_config = config.get_test(test_name_arg);
        if (not test_config) {
          cli::app::error(1, "test {} does not exist", test_name_arg);
        }
      }
    }

    void run_test(const config_test &test, const config_module &file) {}
  };
  struct test_remove_action {
    void operator()(cli::app &app) {
      app.add_argument("--name").optional().help("The name of the tests to remove").multiple();
      app.add_argument("--delete-dir")
        .optional()
        .help("Give to delete the test data folder")
        .as_flag();
      app.add_argument("--strict")
        .optional()
        .help("Check that all tests exists before deleting")
        .as_flag();
      app.parse_argument();
      auto module_dir = get_default_config_module_path();
      bool delete_dir = app.arguments().contains("--delete-dir");
      bool strict = app.arguments().contains("--strict");
      auto config = load_config_module();
      if (strict) {
        for (auto test_name_arg : app.arguments().filter("--name")) {
          if (not config.get_test(test_name_arg)) {
            cli::app::error(1, "test {} is not found", test_name_arg);
          }
        }
      }
      // Remove the tests
      for (auto test_name_arg : app.arguments().filter("--name")) {
        auto deleted_config = config.remove_test(test_name_arg);
        if (not deleted_config) {
          cli::app::warn("test {} is not found", test_name_arg);
        }
        if (delete_dir) {
          fs::remove_all(module_dir / deleted_config->run_yml);
        }
        save_config_module(config);
      }
    }
  };

  export struct test_action {
    void operator()(cli::app &app) {
      app.build_action()
        .add_action("add", "Add tests to the module", test_add_action{})
        .add_action("run", "Run tests already in the module", test_run_action{})
        .add_action("list", "List all the tests available", test_list_action{})
        .add_action("remove", "Remove tests already in the module", test_remove_action{})
        .run();
    }
  };
}