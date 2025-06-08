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
import pharmaco.core.module_;
import moderna.io;
import :loader;

namespace pharmaco::cli {
  namespace fs = std::filesystem;
  namespace cli = moderna::cli;
  namespace proc = moderna::process;
  namespace core = pharmaco::core;
  namespace io = moderna::io;
  namespace py = pybind11;

  export struct run_action {
    rfl::Generic::Object get_params(
      cli::app &app, std::string_view arg_name, rfl::Generic::Object &&converted_params
    ) const {
      auto params = app.arguments().filter(arg_name);
      for (const auto &param : params) {
        auto key_value = proc::env_entry::make(param);
        auto converted_param = cli::app::test_expected(
          rfl::json::read<rfl::Generic>(key_value.value),
          1,
          "Error converting for argument: {3:}, {1:}={2:}: {0:}",
          key_value.name,
          key_value.value,
          arg_name
        );
        converted_params[std::string{key_value.name}] = std::move(converted_param);
      }
      return converted_params;
    }

    void build_container(
      std::string_view container_name,
      const fs::path &work_dir,
      const config_module &config
    ) {
      auto args = config.get_build_arguments(container_name, work_dir);
      cli::app::test_expected(proc::subprocess::run(args));
    }
    void operator()(cli::app &app) {
      app.add_argument("--params")
        .multiple(true)
        .help("Parameters to run the module with, this will override the default parameters. ");
      app.add_argument("--depends")
        .multiple(true)
        .help(
          "Dependencies to run the module with, this will be used to generate the default parameters. "
        );
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
      auto config = load_config_module();
      auto image_name = config.get_image_name();
      auto guard = py::scoped_interpreter{};
      auto manifest = load_python_plugin(app);
      auto module_dir = get_default_module_dir();

      // Clear directory
      if (clear) {
        work_dir.clear_content();
      }

      // Build Container
      if (build) {
        cli::app::out("Building Container");
        build_container(image_name, module_dir, config);
      }
      // Now load and validate parameters
      auto default_parameters = cli::app::test_expected(cli::app::test_expected(
        manifest.get_default_parameters(get_params(app, "--depends", rfl::Generic::Object{}))
      ));
      auto parameters = get_params(app, "--params", std::move(default_parameters));
      auto valid_parameters =
        cli::app::test_expected(cli::app::test_expected(manifest.validate_parameters(parameters)));

      // Now we need to run the module with valid parameters.
      auto run_data = core::module_run_data{
        .work_dir{std::move(work_dir)}, .parameters{valid_parameters}, .container_name{image_name}
      };
      auto proc = cli::app::test_expected(run_data.run_module());
      auto res = cli::app::test_expected(proc.process().wait(false));
      if (log) {
        auto out_content = cli::app::test_expected(
          io::open_file<io::open_mode::read>(run_data.work_dir.stdout_path())
            .and_then([](auto &&f) { return f.read(); })
        );
        auto err_content = cli::app::test_expected(
          io::open_file<io::open_mode::read>(run_data.work_dir.stderr_path())
            .and_then([](auto &&f) { return f.read(); })
        );
        cli::app::out("stdout:\n{}\n\nstderr:\n{}", out_content, err_content);
      }
    };
  };
}