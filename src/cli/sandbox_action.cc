module;
#include <format>
#include <rfl/yaml.hpp>
export module pharmaco.cli:sandbox_action;
import moderna.cli;
import moderna.type_check;

namespace pharmaco::cli {
  namespace cli = moderna::cli;
  namespace tc = moderna::type_check;
  struct type_tester_action {
    void operator()(cli::app &app) {
      app.add_argument("--type").help("The type to test");
      app.add_argument("-v").as_flag().help("Verbosely print if type is valid");
      app.parse_argument();
      auto type_arg = app.test_expected(
        tc::parse_string(app.arguments().first_of("--type").value()), 1, "[x] '{}' is invalid:"
      );
      bool verbose = app.arguments().contains("-v");
      app.out("[√] '{}' is a valid type\n", type_arg);
      if (verbose) {
        app.out("Type Hierarchy Tree:\n{}", rfl::yaml::write(type_arg));
      }
    }
  };
  export struct sandbox_action {
    void operator()(cli::app &app) {
      app.build_action("Use sandbox functions to test configurations")
        .add_action(
          "type_tester", "Test if a string can be parsed into a pharmaco type", type_tester_action{}
        )
        .run();
    }
  };
}