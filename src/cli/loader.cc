module;
#include <pybind11/pybind11.h>
#include <rfl/yaml.hpp>
#include <filesystem>
export module pharmaco.cli:loader;
import moderna.cli;
import pharmaco.manifest;
import :config_module;
/*
  These contains loaders to frequently used functions
*/
namespace pharmaco::cli {
  namespace fs = std::filesystem;
  namespace cli = moderna::cli;
  namespace manifest = pharmaco::manifest;

  fs::path get_default_module_dir() {
    return fs::current_path();
  }
  fs::path get_default_config_module_path() {
    return get_default_module_dir() / "config.yml";
  }

  fs::path get_default_manifest_py_path() {
    return get_default_module_dir() / "manifest.py";
  }

  config_module load_config_module(const fs::path &module_path = get_default_config_module_path()) {
    return cli::app::test_expected(config_module::load_config(module_path));
  }
  void save_config_module(
    const config_module &module, const fs::path &module_path = get_default_module_dir()
  ) {
    return cli::app::test_expected(module.save(module_path));
  }

  manifest::loaded_plugin<manifest::py_plugin> load_python_plugin(cli::app &app) {
    manifest::py_plugin py_module =
      cli::app::test_expected(manifest::py_plugin::load(get_default_manifest_py_path()));
    return cli::app::test_expected(
      manifest::make_plugin<manifest::py_plugin>(std::move(py_module))
    );
  }
}