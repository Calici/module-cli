module;
#include <concepts>
#include <expected>
#include <filesystem>
#include <rfl.hpp>
export module pharmaco.manifest:is_plugin;
import pharmaco.core.zip;
import pharmaco.core.type_schema;
import pharmaco.core.io_nodes;
import :payment;
import :error;
namespace core = pharmaco::core;

/*
  This file declares concept satisfaction for a manifest plugin file that can be written in a lot of
  languages or even with a dylib.
*/

namespace pharmaco::manifest {
  namespace fs = std::filesystem;

  /*
    required_plugin_type
    A must have attribute for a module plugin
  */
  export template <class return_type>
  using required_plugin_type = std::expected<return_type, plugin_error>;

  /*
    optional_plugin_type
    An optional attribute for a module plugin
  */
  export template <class return_type>
  using optional_plugin_type = std::expected<std::optional<return_type>, plugin_error>;

  /*
    Type Defs
  */
  using parameter_type = rfl::Generic::Object;
  using result_type = rfl::Generic::Object;
  using depends_type = rfl::Generic::Object;
  struct empty_plugin_result {};

  /*
    Future Implementations can only add optional attributes to avoid breaking old code.
    This is the constraint requirement for a plugin.
  */
  export template <class plugin_type>
  concept is_plugin = requires(
    const plugin_type plugin,
    const parameter_type &params,
    const depends_type &depends,
    core::type_schema_validation_error &errors,
    std::optional<std::reference_wrapper<const result_type>> results,
    const fs::path &work_dir,
    core::zip_builder &builder
  ) {
    // Required Attributes
    /*
      Gets the default parameters for a specific module
    */
    { plugin.get_default_parameters(params) } -> std::same_as<required_plugin_type<parameter_type>>;
    /*
      Gets the parameter schema for a specific module, this will be used to validate data types.
    */
    {
      plugin.get_params_schema()
    } -> std::same_as<required_plugin_type<core::typed_record_type_schema>>;
    /*
      Gets the results schema for a specific module.
    */
    {
      plugin.get_results_schema()
    } -> std::same_as<required_plugin_type<core::typed_record_type_schema>>;
    /*
      Gets the in nodes schema. This will be used to validate nodes coming from other modules.
    */
    {
      plugin.get_in_nodes_schema()
    } -> std::same_as<required_plugin_type<core::typed_record_type_schema>>;
    /*
      Gets the input nodes for a specific module. This will be used for dynamic connection.
      Validation over its content should be done separately.
    */
    { plugin.get_in_nodes() } -> std::same_as<required_plugin_type<core::io_nodes>>;
    /*
      Get the output nodes for a specific module. This will be used for dynamic connection.
      Validation over its content should be done separately.
    */
    { plugin.get_out_nodes() } -> std::same_as<required_plugin_type<core::io_nodes>>;

    // Optional Attributes
    /*
      Validates the dependencies going into the current module.
    */
    {
      plugin.validate_in_nodes(depends, errors)
    } -> std::same_as<optional_plugin_type<std::optional<depends_type>>>;
    /*
      Validates the parameters after the user have inputted all the required parameters.
    */
    {
      plugin.validate_parameters(params, errors)
    } -> std::same_as<optional_plugin_type<std::optional<parameter_type>>>;
    /*
      Gets the required token based on the current parameters
    */
    { plugin.get_required_token(params) } -> std::same_as<optional_plugin_type<int>>;
    /*
      Gets the download zip based on the current working directory.
    */
    {
      plugin.get_download_zip(builder, work_dir)
    } -> std::same_as<optional_plugin_type<empty_plugin_result>>;
  };

  export template <class plugin_type> 
  concept is_reloadable_plugin = requires(const plugin_type plugin) {
    { plugin.reload() } -> std::same_as<std::expected<void, plugin_error>>;
  };
}