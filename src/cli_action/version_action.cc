module;
#include <optional>
export module pharmaco.cli:version_action;
import moderna.cli;
import :config;

namespace pharmaco::cli {
  namespace cli = moderna::cli;

  struct version_list_action {
    void operator()(cli::app &app) {
      auto file =
        app.test_expected(module_config_file::load(), 1, "Error opening module config: {}");
      app.out("Versions List: ");
      for (size_t i = 0; i != file.version_size(); i++) {
        app.out("{}", file.get_version(i).value().get());
      }
    }
  };

  struct version_add_action {
    cli::app_version validate_version(cli::app &app) {
      auto res = app.arguments().first_of("--version").and_then(cli::app_version::from_string);
      if (!res) {
        app.error(1, "Invalid version string {}", app.arguments().first_of("--version").value());
      }
      return res.value();
    }
    void operator()(cli::app &app) {
      app.add_argument("--version").help("The new version").required();
      app.add_argument("--description").help("The description for the new version").required();
      app.add_argument("--no-beta")
        .help("Sets the is_beta tag to false for the current version")
        .as_flag()
        .optional();
      app.add_argument("--tag")
        .help("The git tag to tag the version branch with on finalisation")
        .optional();
      app.parse_argument();
      auto file =
        app.test_expected(module_config_file::load(), 1, "Error opening module config: {}");
      auto new_version = validate_version(app);
      auto description = app.arguments().first_of("--description").value();
      auto is_beta = !app.arguments().contains("--not_beta");
      auto tag = app.arguments().first_of("--tag");
      /*
        Check existence
      */
      auto add_res = file.add_version(new_version, description, is_beta, tag);
      if (not add_res) {
        app.error(1, "Version {} have already been previously added", new_version);
      }
      app.test_expected(file.save(), 1, "Error saving module config: {}");
      app.out("Added version {}", add_res.value().get());
    }
  };

  struct version_config_action {
    std::pair<std::optional<bool>, std::optional<bool>> validate_arguments(cli::app &app) {
      if (app.arguments().contains("--allow") && app.arguments().contains("--no-allow")) {
        app.error(1, "--allow and --no-allow cannot be simultaneously given");
      } else if (app.arguments().contains("--beta") && app.arguments().contains("--no-beta")) {
        app.error(1, "--beta and --no-beta cannot be simultaneously given");
      }
      std::optional<bool> is_beta;
      std::optional<bool> allow_use;
      if (app.arguments().contains("--beta")) is_beta = true;
      else if (app.arguments().contains("--no-beta"))
        is_beta = false;
      if (app.arguments().contains("--allow")) allow_use = true;
      else if (app.arguments().contains("--no-allow"))
        allow_use = false;
      return std::pair{is_beta, allow_use};
    }
    version_config get_version(cli::app &app, const module_config_file &file) {
      auto parsed_version =
        app.arguments().first_of("--version").and_then(cli::app_version::from_string);
      if (not parsed_version) {
        app.error(1, "Invalid version \"{}\"", app.arguments().first_of("-v").value());
      }
      auto version = file.get_version(parsed_version.value());
      if (not version) {
        app.error(1, "Version \"{}\" is not in the config file", parsed_version.value());
      }
      return version.value();
    }
    void operator()(cli::app &app) {
      app.add_argument("--version").help("The version to configure").required();
      app.add_argument("--description")
        .help("Change the description of the specified version")
        .optional();
      app.add_argument("--allow").help("Allow module use").as_flag().optional();
      app.add_argument("--no-allow").help("Disallow module use").as_flag().optional();
      app.add_argument("--beta").help("Set the specified version to beta").as_flag().optional();
      app.add_argument("--no-beta")
        .help("Set the specified version to not beta")
        .as_flag()
        .optional();
      app.add_argument("--tag").help("Set a new tag for the specified version").optional();
      app.parse_argument();
      auto file =
        app.test_expected(module_config_file::load(), 1, "Error opening module config: {}");
      auto [is_beta, allow_use] = validate_arguments(app);
      auto description = app.arguments().first_of("--description");
      auto tag = app.arguments().first_of("--tag");
      auto version = get_version(app, file);
      if (is_beta) {
        version.is_beta = is_beta.value();
      }
      if (allow_use) {
        version.allow_use = allow_use.value();
      }
      if (description) {
        version.description = description.value();
      }
      if (tag) {
        version.git_tag = tag.value();
      }
      auto set_res = file.set_version(version);
      app.test_expected(file.save(), 1, "Error saving module config: {}");
      app.out("Configured version {}", set_res.value().get());
    }
  };

  export struct version_action {
    void operator()(cli::app &app) {
      cli::app_actions::empty()
        .add_action("list", "Lists all the available version", version_list_action{})
        .add_action("add", "Adds a new version to the available version", version_add_action{})
        .add_action(
          "config", "Configures the behaviour of a specific version", version_config_action{}
        )
        .apply_and_run(app);
    }
  };
}