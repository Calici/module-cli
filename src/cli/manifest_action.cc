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
import :loader;

namespace pharmaco::cli {
  namespace cli = moderna::cli;
  namespace fs = std::filesystem;
  namespace proc = moderna::process;
  namespace manifest = pharmaco::manifest;
  namespace py = pybind11;

  struct manifest_validate_action {
    rfl::Generic::Object get_params(
      cli::app &app, std::string_view arg_name, rfl::Generic::Object &&converted_params
    ) const {
      auto params = app.arguments().filter(arg_name);
      for (const auto &param : params) {
        auto key_value = proc::env_entry::make(param);
        auto converted_param = rfl::json::read<rfl::Generic>(key_value.value);
        if (converted_param) {
          converted_params[std::string{key_value.name}] = std::move(converted_param.value());
        } else {
          converted_params[std::string{key_value.name}] = std::string{key_value.value};
        }
      }
      return std::move(converted_params);
    }
    template <manifest::is_plugin plugin_type>
    rfl::Generic::Object get_default_parameters(
      cli::app &app, manifest::loaded_plugin<plugin_type> &plugin
    ) const {
      return cli::app::test_expected(
        cli::app::test_expected(
          plugin.get_default_parameters(get_params(app, "--depends", rfl::Generic::Object{}))
        )
      );
    }
    template <manifest::is_plugin plugin_type>
    auto validate_parameters(
      cli::app &app,
      manifest::loaded_plugin<plugin_type> &plugin,
      const rfl::Generic::Object &parameters
    ) {
      auto valid_parameter_res = cli::app::test_expected(plugin.validate_parameters(parameters));
      return cli::app::test_expected(std::move(valid_parameter_res));
    }
    void operator()(cli::app &app) {
      app.add_argument("--params")
        .help(
          "Parameters to use for validation in the form of KEY=VALUE. This will be used to override the default parameters"
        )
        .multiple();
      app.add_argument("--depends")
        .help("Dependencies to use to craft the default parameters")
        .multiple();
      app.parse_argument();

      /*
        Begin py interpreter
      */
      auto guard = py::scoped_interpreter{};
      auto plugin = load_python_plugin(app);
      auto default_parameters = get_default_parameters(app, plugin);
      auto parameters = get_params(app, "--params", std::move(default_parameters));
      auto valid_parameters = validate_parameters(app, plugin, parameters);
      cli::app::out("{}", rfl::yaml::write(valid_parameters));
    }
  };

  struct manifest_print_action {
    void operator()(cli::app &app) {
      /*
        Begin py interpreter
      */
      auto guard = py::scoped_interpreter{};
      auto plugin = load_python_plugin(app);
      cli::app::out(
        "Parameters:\n{}\n\nResults:\n{}\n\nIn Nodes Schema:\n{}\n\nIn Nodes:\n{}\nOut Nodes:\n{}\n",
        plugin.io().params_schema,
        plugin.io().results_schema,
        plugin.io().in_nodes_schema,
        plugin.io().in_nodes,
        plugin.io().out_nodes
      );
    }
  };

  struct get_params_schema_action {
    void operator()(cli::app &app) {
      auto guard = py::scoped_interpreter{};
      auto plugin = load_python_plugin(app);
      cli::app::out("Params Schema:\n{}", plugin.io().params_schema);
    }
  };

  struct get_results_schema_action {
    void operator()(cli::app &app) {
      auto guard = py::scoped_interpreter{};
      auto plugin = load_python_plugin(app);
      cli::app::out("Results Schema:\n{}", plugin.io().results_schema);
    }
  };

  struct get_in_nodes_action {
    void operator()(cli::app &app) {
      auto guard = py::scoped_interpreter{};
      auto plugin = load_python_plugin(app);
      cli::app::out("Out Nodes: \n{}", plugin.io().out_nodes);
    }
  };
  struct get_out_nodes_action {
    void operator()(cli::app &app) {
      auto guard = py::scoped_interpreter{};
      auto plugin = load_python_plugin(app);
      cli::app::out("In Nodes: \n{}", plugin.io().in_nodes);
    }
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
        cli::app::warn("{} is not .zip", output.extension().c_str());
      }
      auto guard = py::scoped_interpreter{};
      auto plugin = load_python_plugin(app);
      cli::app::test_expected(plugin.get_download_zip(output, work_dir));
    }
  };
  struct get_in_nodes_schema_action {
    void operator()(cli::app &app) {
      auto guard = py::scoped_interpreter{};
      auto plugin = load_python_plugin(app);
      cli::app::out("In Nodes Schema: \n{}", plugin.io().in_nodes_schema);
    }
  };

  struct manifest_test_action {
    void operator()(cli::app &app) {
      app.build_action()
        .add_action("get_params_schema", "Check get_params_schema", get_params_schema_action{})
        .add_action("get_results_schema", "Check get_results_schema", get_results_schema_action{})
        .add_action("get_in_nodes_schema", "Check in_nodes schema", get_in_nodes_schema_action{})
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