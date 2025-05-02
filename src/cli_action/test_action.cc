module;
#include <filesystem>
export module pharmaco.cli:test_action;
import moderna.cli;
import :config;

namespace pharmaco::cli {
  namespace cli = moderna::cli;
  namespace fs = std::filesystem;
  struct test_add_action {
    void operator()(cli::app &app) {
      app.add_argument("--name").help("The new test name").required();
      app.add_argument("--data").help("The path to the test data").required();
      app.parse_argument();

      auto config = app.test_expected(module_config_file::load(), 1, "Error opening file: {}");
      auto name = app.arguments().first_of("--name").value();
      auto data = app.arguments().first_of("--data").value();
      auto added_config = config.add_test(name, data);
      if (not added_config) {
        app.error(1, "Error: test with name {} already exists", name);
      }
      app.test_expected(config.save(), 1, "Error saving module config: {}");
      app.out("Test {} added", name);
    }
  };
  struct test_list_action {
    void operator()(cli::app &app) {
      auto config =
        app.test_expected(module_config_file::load(), 1, "Error opening module configs: {}");
      app.out("Available Tests :");
      for (const auto &test_config : config.config().tests) {
        app.out("- {:16} at {}", test_config.name, test_config.test_data.c_str());
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
      auto file =
        app.test_expected(module_config_file::load(), 1, "Error opening module config: {}");
      if (app.arguments().args().count("--name") == 0) {
        for (const auto &test : file.config().tests) {
          run_test(test, file);
        }
      } else {
        if (strict) {
          validate_test_names(file, app);
        }
        for (const auto &test_name_arg : app.arguments().args().filter("--name")) {
          auto test_config = file.get_test(test_name_arg.value);
          if (test_config) {
            run_test(*test_config, file);
          } else {
            app.warn("WARN: {} cannot be found", test_name_arg.value);
          }
        }
      }
    }
    void validate_test_names(const module_config_file &file, const cli::app &app) {
      for (const auto &test_name_arg : app.arguments().args().filter("--name")) {
        auto test_config = file.get_test(test_name_arg.value);
        if (not test_config) {
          app.error(1, "Test {} does not exist", test_name_arg.value);
        }
      }
    }

    void run_test(const test_config &test, const module_config_file &file) {}
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

      auto config = app.test_expected(module_config_file::load(), 1, "Error opening file: {}");
      bool delete_dir = app.arguments().contains("--delete-dir");
      bool strict = app.arguments().contains("--strict");
      if (strict) {
        for (auto test_name_arg : app.arguments().args().filter("--name")) {
          if (not config.get_test(test_name_arg.value)) {
            app.error(1, "Test {} is not found", test_name_arg.value);
          }
        }
      }
      // Remove the tests
      for (auto test_name_arg : app.arguments().args().filter("--name")) {
        auto deleted_config = config.remove_test(test_name_arg.value);
        if (not deleted_config) {
          app.warn("Test {} is not found", test_name_arg.value);
        }
        if (delete_dir) {
          fs::remove_all(config.dir() / deleted_config->test_data);
        }
        app.test_expected(config.save(), 1, "Error saving module config : {}");
      }
    }
  };

  export struct test_action {
    void operator()(cli::app &app) {
      cli::app_actions::empty()
        .add_action("add", "Add tests to the module", test_add_action{})
        .add_action("run", "Run tests already in the module", test_run_action{})
        .add_action("list", "List all the tests available", test_list_action{})
        .add_action("remove", "Remove tests already in the module", test_remove_action{})
        .apply_and_run(app);
    }
  };
}