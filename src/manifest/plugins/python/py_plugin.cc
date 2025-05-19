module;
#include <pybind11/pybind11.h>
#include <expected>
#include <filesystem>
#include <optional>
#include <string_view>
export module pharmaco.manifest:plugins_python_py_plugin;
import :plugins_python_function_module;
import :zip_builder;
import :plugins_python_conversions;
import :io_nodes;
import :type_schema;
import :is_plugin;

namespace pharmaco::manifest {
  namespace fs = std::filesystem;
  namespace py = pybind11;

  export class py_plugin {
    function_module __module;

  public:
    py_plugin(function_module module) : __module{std::move(module)} {}

    required_plugin_type<parameter_type> get_default_parameters() const {
      return __module.run_required_function<parameter_type>("get_default_parameters");
    }
    required_plugin_type<typed_record_type_schema> get_params_schema() const {
      return __module.run_required_function<typed_record_type_schema>("get_params_schema");
    }
    required_plugin_type<typed_record_type_schema> get_results_schema() const {
      return __module.run_required_function<typed_record_type_schema>("get_results_schema");
    }
    required_plugin_type<io_nodes> get_in_nodes() const {
      return __module.run_required_function<io_nodes>("get_in_nodes", io_nodes::make_empty());
    }
    required_plugin_type<io_nodes> get_out_nodes() const {
      auto out_nodes = io_nodes::make_empty();
      return __module.run_required_function<io_nodes>("get_out_nodes", io_nodes::make_empty());
    }

    // Optional Attributes
    optional_plugin_type<std::expected<parameter_type, type_schema_validation_error>>
    validate_parameters(const parameter_type &parameters) const {
      using expected_type = std::expected<parameter_type, type_schema_validation_error>;
      auto errors = type_schema_validation_error::make_empty();
      return __module
        .run_function<std::optional<parameter_type>>(
          "validate_parameters", parameters, std::ref(errors)
        )
        .transform([&](auto &&optional_parameters) {
          return optional_parameters.transform([&](auto &&func_result) {
            return func_result
              .transform([](auto &&params) { return expected_type{std::move(params)}; })
              .value_or(expected_type{std::unexpected{std::move(errors)}});
          });
        });
    }
    optional_plugin_type<int> get_required_token(const parameter_type &parameters) const {
      return __module.run_function<int>("get_required_token", parameters);
    }
    optional_plugin_type<empty_plugin_result> get_download_zip(
      zip_builder &builder, const fs::path &work_dir
    ) const {
      return __module.run_function<py::object>("get_download_zip", std::ref(builder), work_dir)
        .transform([](auto &&) { return std::optional<empty_plugin_result>{empty_plugin_result{}}; }
        );
    }

    static std::expected<py_plugin, py::error_already_set> load(std::string_view module_name) {
      return function_module::load(module_name).transform([](auto &&module) {
        return py_plugin{std::move(module)};
      });
    }
  };
  static_assert(is_plugin<py_plugin>);
}