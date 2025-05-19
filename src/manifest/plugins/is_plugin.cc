module;
#include <concepts>
#include <expected>
#include <filesystem>
#include <rfl.hpp>
export module pharmaco.manifest:is_plugin;
import pharmaco.core;
import :io_nodes;
import :error;
import :type_schema;
import :payment;
import :zip_builder;

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
  struct empty_plugin_result {};

  /*
    Future Implementations can only add optional attributes to avoid breaking old code.
  */
  export template <class plugin_type>
  concept is_plugin = requires(
    const plugin_type plugin,
    const parameter_type &params,
    std::optional<std::reference_wrapper<const result_type>> results,
    const fs::path &work_dir,
    zip_builder &builder
  ) {
    // Required Attributes
    { plugin.get_default_parameters() } -> std::same_as<required_plugin_type<parameter_type>>;
    { plugin.get_params_schema() } -> std::same_as<required_plugin_type<typed_record_type_schema>>;
    { plugin.get_results_schema() } -> std::same_as<required_plugin_type<typed_record_type_schema>>;
    { plugin.get_in_nodes() } -> std::same_as<required_plugin_type<io_nodes>>;
    { plugin.get_out_nodes() } -> std::same_as<required_plugin_type<io_nodes>>;

    // Optional Attributes
    {
      plugin.validate_parameters(params)
    } -> std::same_as<
      optional_plugin_type<std::expected<parameter_type, type_schema_validation_error>>>;
    { plugin.get_required_token(params) } -> std::same_as<optional_plugin_type<int>>;
    {
      plugin.get_download_zip(builder, work_dir)
    } -> std::same_as<optional_plugin_type<empty_plugin_result>>;
  };

}