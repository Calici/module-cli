module;
#include <pybind11/embed.h>
#include <rfl/Generic.hpp>
#include <rfl/json.hpp>
#include <rfl/yaml.hpp>
#include <filesystem>
#include <string>
export module pharmaco.cli:manifest_action;
import moderna.cli;
import moderna.process;
import pharmaco.manifest;
import :config;
import :loader;

namespace pharmaco::cli {
  namespace cli = moderna::cli;
  namespace fs = std::filesystem;
  namespace proc = moderna::process;
  namespace manifest = pharmaco::manifest;
  namespace py = pybind11;

  struct manifest_validate_action {
    rfl::Generic::Object get_params(cli::app &app, rfl::Generic::Object &&converted_params) const {
      auto params = app.arguments().filter("--params");
      for (const auto &param : params) {
        auto key_value = proc::env_entry::make(param);
        auto converted_param = app.test_expected(
          rfl::json::read<rfl::Generic>(key_value.value),
          1,
          "Error converting param {1:}={2:}: {0:}",
          key_value.name,
          key_value.value
        );
        converted_params[std::string{key_value.name}] = std::move(converted_param);
      }
      return converted_params;
    }
    template <manifest::is_plugin plugin_type>
    auto get_default_parameters(cli::app &app, manifest::loaded_plugin<plugin_type> &plugin) const {
      return app.test_expected(plugin.get_default_parameters());
    }
    template <manifest::is_plugin plugin_type>
    auto validate_parameters(
      cli::app &app,
      manifest::loaded_plugin<plugin_type> &plugin,
      const rfl::Generic::Object &parameters
    ) {
      auto valid_parameter_res = app.test_expected(plugin.validate_parameters(parameters));
      return app.test_expected(std::move(valid_parameter_res));
    }
    void operator()(cli::app &app) {
      app.add_argument("--params")
        .help(
          "Parameters to use for validation in the form of KEY=VALUE. This will be used to override the default parameters"
        )
        .multiple();
      app.parse_argument();
      auto config = load_module_config(app);
      fs::path manifest_path = config.config().manifest_file;

      /*
        Begin py interpreter
      */
      auto guard = py::scoped_interpreter{};
      auto plugin = load_python_plugin(app);
      auto default_parameters = get_default_parameters(app, plugin);
      auto parameters = get_params(app, std::move(default_parameters));
      auto valid_parameters = validate_parameters(app, plugin, parameters);
      app.out("{}", rfl::yaml::write(valid_parameters));
    }
  };

  struct manifest_print_action {
    void operator()(cli::app &app) {
      auto config = load_module_config(app);
      fs::path manifest_path = config.config().manifest_file;

      /*
        Begin py interpreter
      */
      auto guard = py::scoped_interpreter{};
      auto plugin = load_python_plugin(app);
      app.out(
        "Parameters:\n{}\n\nResults:\n{}\n\nIn Nodes:\n{}\nOut Nodes:\n{}\n",
        plugin.params(),
        plugin.results(),
        plugin.in_nodes(),
        plugin.out_nodes()
      );
    }
  };

  struct manifest_download_path {
    void operator()(cli::app &app) {}
  };

  struct get_params_schema_action {
    void operator()(cli::app &app) {}
  };

  struct get_results_schema_action {
    void operator()(cli::app &app) {}
  };

  struct get_in_nodes_action {
    void operator()(cli::app &app) {}
  };
  struct get_out_nodes_action {
    void operator()(cli::app &app) {}
  };
  struct get_download_zip_action {
    void operator()(cli::app &app) {
      app.add_argument("--work_dir")
        .help("The work directory to use for download zip creation")
        .required();
      app.add_argument("--output").help("The zip file output to use").required();
      app.parse_argument();
      auto work_dir = app.arguments()
                        .first_of("--work_dir")
                        .transform([](cli::argument_value &&value) {
                          return fs::absolute(value.cast<fs::path>().value());
                        })
                        .value();
      auto output = app.arguments()
                      .first_of("--output")
                      .transform([](cli::argument_value &&value) {
                        return fs::absolute(value.cast<fs::path>().value());
                      })
                      .value();
      if (output.extension() != ".zip") {
        app.warn("{} is not .zip", output.extension().c_str());
      }
      auto guard = py::scoped_interpreter{};
      auto plugin = load_python_plugin(app);
      app.test_expected(plugin.get_download_zip(output, work_dir));
    }
  };
  struct manifest_test_action {
    void operator()(cli::app &app) {
      app.build_action()
        .add_action("get_params_schema", "Check get_params_schema", get_params_schema_action{})
        .add_action("get_results_schema", "Check get_results_schema", get_results_schema_action{})
        .add_action("get_in_nodes", "Check get_in_nodes", get_in_nodes_action{})
        .add_action("get_out_nodes", "Check get_out_nodes", get_out_nodes_action{})
        .add_action("get_download_zip", "Get the zip file to download", get_download_zip_action{})
        .run();
    }
  };
  export struct manifest_action {
    void operator()(cli::app &app) const {
      app.build_action()
        .add_action("validate", "Validates the given manifest file", manifest_validate_action{})
        .add_action("test", "Tests a given functionality of the plugin", manifest_test_action{})
        .add_action(
          "show_config",
          "Shows the parameters, results, in_nodes and out_nodes",
          manifest_print_action{}
        )
        .run();
    }
  };

}