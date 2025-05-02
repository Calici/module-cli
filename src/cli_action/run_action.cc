module;
export module pharmaco.cli:run_action;
import moderna.cli;
import moderna.process;
import :config;

namespace pharmaco::cli {
  namespace cli = moderna::cli;
  namespace proc = moderna::process;

  export struct run_action {
    void operator()(cli::app &app) {
      app.add_argument("--params")
        .multiple(true)
        .help(
          "The parameters to run the module with. Validation will be performed against the manifest file"
        );
      app.add_argument("--work_dir").required().help("The working directory to use");
      // Find work directory.
      auto file =
        app.test_expected(module_config_file::load(), 1, "Error opening module config: {}");
    }
  };
}