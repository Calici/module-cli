module;
#include <pybind11/pybind11.h>
#include <expected>
#include <filesystem>
#include <optional>
#include <rfl.hpp>
#include <string_view>
export module pharmaco.manifest:plugins_python_py_plugin;
import pharmaco.core.bindings.python;
import pharmaco.core.zip;
import pharmaco.core.type_schema;
import pharmaco.core.io_nodes;
import :is_plugin;

namespace pharmaco::manifest {
  namespace fs = std::filesystem;
  namespace py = pybind11;
  namespace core = pharmaco::core;
  namespace core_py = pharmaco::core::python;
  export struct py_module_loader {
    void operator()(py::module_ &m) const {
      auto sub_m = m.def_submodule("core");
      core_py::load_python_module(
        sub_m,
        core::zip_python_loader{}, // core.zip
        core::type_schema_python_loader{}, // core.type_schema
        core::io_nodes_python_loader{} // core.io_nodes
      );
    }
  };
  export class py_plugin {
    core_py::function_module<plugin_error> __module;

  public:
    py_plugin(core_py::function_module<plugin_error> module) : __module{std::move(module)} {}

    required_plugin_type<parameter_type> get_default_parameters(
      const rfl::Generic::Object &depends
    ) const {
      return __module.run_required_function<parameter_type>("get_default_parameters", depends);
    }
    required_plugin_type<core::typed_record_type_schema> get_params_schema() const {
      return __module.run_required_function<core::typed_record_type_schema>("get_params_schema");
    }
    required_plugin_type<core::typed_record_type_schema> get_results_schema() const {
      return __module.run_required_function<core::typed_record_type_schema>("get_results_schema");
    }
    required_plugin_type<core::typed_record_type_schema> get_in_nodes_schema() const {
      return __module.run_required_function<core::typed_record_type_schema>("get_in_nodes_schema");
    }
    required_plugin_type<core::io_nodes> get_in_nodes() const {
      return __module.run_required_function<core::io_nodes>(
        "get_in_nodes", core::io_nodes::make_empty()
      );
    }
    required_plugin_type<core::io_nodes> get_out_nodes() const {
      auto out_nodes = core::io_nodes::make_empty();
      return __module.run_required_function<core::io_nodes>(
        "get_out_nodes", core::io_nodes::make_empty()
      );
    }

    // Optional Attributes
    optional_plugin_type<std::optional<depends_type>> validate_in_nodes(
      const depends_type &depends, core::type_schema_validation_error &errors
    ) const {
      return __module.run_function<std::optional<depends_type>>(
        "validate_in_nodes", depends, std::ref(errors)
      );
    }
    optional_plugin_type<std::optional<parameter_type>> validate_parameters(
      const parameter_type &parameters, core::type_schema_validation_error &errors
    ) const {
      return __module.run_function<std::optional<parameter_type>>(
        "validate_parameters", parameters, std::ref(errors)
      );
    }
    optional_plugin_type<int> get_required_token(const parameter_type &parameters) const {
      return __module.run_function<int>("get_required_token", parameters);
    }
    optional_plugin_type<empty_plugin_result> get_download_zip(
      core::zip_builder &builder, const fs::path &work_dir
    ) const {
      return __module.run_function<py::object>("get_download_zip", std::ref(builder), work_dir)
        .transform([](auto &&) {
          return std::optional<empty_plugin_result>{empty_plugin_result{}};
        });
    }

    // Python Module specific
    std::expected<void, plugin_error> reload() const {
      return __module.reload().transform_error([](auto &&e) {
        return plugin_error{"reload", e.what()};
      });
    }

    static std::expected<py_plugin, py::error_already_set> load(const fs::path &module_path) {
      return core_py::function_module<plugin_error>::load(module_path).transform([](auto &&module) {
        return py_plugin{std::move(module)};
      });
    }
  };
  static_assert(is_plugin<py_plugin>);
}