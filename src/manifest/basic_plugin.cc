module;
#include <expected>
#include <filesystem>
export module pharmaco.manifest:basic_plugin;
import pharmaco.core.zip;
import pharmaco.core.type_schema;
import pharmaco.core.io_nodes;
import :is_plugin;
import :error;

namespace core = pharmaco::core;

namespace pharmaco::manifest {
  namespace fs = std::filesystem;

  struct basic_plugin {
    virtual required_plugin_type<core::typed_record_type_schema> get_params_schema() const = 0;
    virtual required_plugin_type<core::typed_record_type_schema> get_results_schema() const = 0;
    virtual required_plugin_type<core::typed_record_type_schema> get_in_nodes_schema() const = 0;
    virtual required_plugin_type<core::io_nodes> get_in_nodes() const = 0;
    virtual required_plugin_type<core::io_nodes> get_out_nodes() const = 0;
    virtual required_plugin_type<parameter_type> get_default_parameters(
      const parameter_type &
    ) const = 0;

    // Optionals
    virtual optional_plugin_type<parameter_type> validate_parameters(
      const parameter_type &, core::type_schema_validation_error &
    ) const = 0;
    virtual optional_plugin_type<depends_type> validate_in_nodes(
      const depends_type &, core::type_schema_validation_error &
    ) const = 0;
    virtual optional_plugin_type<int> get_required_token(const parameter_type &) const = 0;
    virtual optional_plugin_type<empty_plugin_result> get_download_zip(
      core::zip_builder &, const fs::path &
    ) const = 0;
  };

  struct reloadable_plugin {
    virtual std::expected<void, plugin_error> reload() const = 0;
  };
}