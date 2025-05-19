module;
#include <pybind11/embed.h>
#include <rfl/json.hpp>
#include <rfl/yaml.hpp>
#include <filesystem>
#include <format>
export module pharmaco.cli:run_action;
import pharmaco.manifest;
import moderna.cli;
import moderna.process;
import pharmaco.core;
import moderna.io;
import :config;
import :loader;

namespace pharmaco::cli {
  namespace fs = std::filesystem;
  namespace cli = moderna::cli;
  namespace proc = moderna::process;
  namespace core = pharmaco::core;
  namespace manifest = pharmaco::manifest;
  namespace io = moderna::io;
  namespace py = pybind11;

  export struct run_action {
    rfl::Generic::Object get_parameters(cli::app &app, rfl::Generic::Object converted_parameters) {
      auto params = app.arguments().filter("--params");
      for (const auto &param : params) {
        auto key_value = proc::env_entry::make(param);
        auto converted_parameter = app.test_expected(
          rfl::json::read<rfl::Generic>(key_value.value),
          1,
          "Error converting param {1:}: {0:}",
          param
        );
        converted_parameters[std::string{key_value.name}] = std::move(converted_parameter);
      }
      return converted_parameters;
    }

    void build_container(
      cli::app &app, std::string_view container_name, const module_config_file &config
    ) {
      app.test_expected(config.config().container.build_container(container_name));
    }
    void operator()(cli::app &app) {
      app.add_argument("--params")
        .multiple(true)
        .help("Parameters to run the module with, this will override the default parameters. ");
      app.add_argument("--work_dir").required().help("The working directory to use");
      app.add_argument("--log").optional().as_flag().help("Pour log output to terminal on error");
      app.add_argument("--clear").optional().as_flag().help("Clear directory before running");
      app.add_argument("--build").optional().as_flag().help("Build the container before running");
      app.parse_argument();
      // Find work directory.
      bool log = app.arguments().contains("--log");
      bool clear = app.arguments().contains("--clear");
      bool build = app.arguments().contains("--build");
      auto work_dir = app.arguments()
                        .first_of("--work_dir")
                        .and_then([](auto &&v) {
                          auto path = fs::absolute(v.template cast<fs::path>().value());
                          return core::module_dir::make_dir(path);
                        })
                        .value();
      auto config = load_module_config(app);
      auto container_name = config.container_name();
      auto guard = py::scoped_interpreter{};
      auto manifest = load_python_plugin(app);

      // Clear directory
      if (clear) {
        work_dir.clear_content();
      }

      // Build Container
      if (build) {
        app.out("Building Container");
        build_container(app, container_name, config);
      }
      // Now load and validate parameters
      auto default_parameters = app.test_expected(manifest.get_default_parameters());
      auto parameters = get_parameters(app, std::move(default_parameters));
      auto valid_parameters =
        app.test_expected(app.test_expected(manifest.validate_parameters(parameters)));

      // Now we need to run the module with valid parameters.
      auto run_data = core::module_run_data{
        .work_dir{std::move(work_dir)},
        .parameters{valid_parameters},
        .container_name{container_name}
      };
      auto proc = app.test_expected(run_data.run_module());
      auto res = app.test_expected(proc.process().wait(false));
      if (log) {
        auto out_content = app.test_expected(
          io::open_file<io::open_mode::read>(run_data.work_dir.stdout_path())
            .and_then([](auto &&f) { return f.read(); })
        );
        auto err_content = app.test_expected(
          io::open_file<io::open_mode::read>(run_data.work_dir.stderr_path())
            .and_then([](auto &&f) { return f.read(); })
        );
        app.out("stdout:\n{}\n\nstderr:\n{}", out_content, err_content);
      }
    };
  };
}