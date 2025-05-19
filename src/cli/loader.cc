module;
#include <filesystem>
#include <pybind11/pybind11.h>
export module pharmaco.cli:loader;
import moderna.cli;
import pharmaco.manifest;
import :config;
/*
  These contains loaders to frequently used functions
*/
namespace pharmaco::cli {
  namespace fs = std::filesystem;
  namespace cli = moderna::cli;
  namespace manifest = pharmaco::manifest;

  fs::path default_module_config_path() {
    return fs::current_path() / "config.yml";
  }

  module_config_file load_module_config(
    cli::app &app, const fs::path &config_path = default_module_config_path()
  ) {
    return app.test_expected(module_config_file::load(config_path));
  }

  manifest::loaded_plugin<manifest::py_plugin> load_python_plugin(
    cli::app &app, std::string_view module_name = "manifest"
  ) {
    manifest::py_plugin py_module = app.test_expected(manifest::py_plugin::load(module_name));
    return app.test_expected(manifest::make_plugin<manifest::py_plugin>(std::move(py_module)));
  }
}