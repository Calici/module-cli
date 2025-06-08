module;
#include <rfl/Generic.hpp>
#include <algorithm>
#include <expected>
#include <filesystem>
export module pharmaco.manifest:loaded_plugin;
import pharmaco.core.zip;
import pharmaco.core.type_schema;
import pharmaco.core.io_nodes;
import :is_plugin;
import :plugin_io;
import :error;

namespace core = pharmaco::core;
namespace pharmaco::manifest {

  export template <is_plugin plugin_type> class loaded_plugin {
    plugin_type __plugin;
    plugin_io __io;

  public:
    loaded_plugin(plugin_type plugin, plugin_io io) :
      __plugin{std::move(plugin)}, __io{std::move(io)} {}

    const plugin_type &plugin() const noexcept {
      return __plugin;
    }

    /*
      Required Attributes
    */
    const plugin_io &io() const noexcept {
      return __io;
    }
    /*
      In Nodes Validation
    */

    /*
      Makes the assumption that in_nodes is type valid and run the plugin function to validate
      in nodes.
    */
    required_plugin_type<std::expected<depends_type, core::type_schema_validation_error>>
    plugin_validate_in_nodes(const depends_type &in_nodes) const {
      using result_type = std::expected<depends_type, core::type_schema_validation_error>;
      auto errors = core::type_schema_validation_error::make_empty();
      return plugin()
        .validate_in_nodes(in_nodes, errors)
        .transform([&](std::optional<std::optional<depends_type>> &&res) {
          if (!res) {
            return result_type{in_nodes};
          } else if (!res->has_value()) {
            return result_type{std::unexpected{std::move(errors)}};
          } else {
            return __io.schema_validate_in_nodes(res->value());
          }
        });
    }
    /*
      Runs in_nodes validation, both schema (type) and plugin
    */
    required_plugin_type<std::expected<depends_type, core::type_schema_validation_error>>
    validate_in_nodes(const depends_type &in_nodes) const {
      using result_type = std::expected<depends_type, core::type_schema_validation_error>;
      result_type type_valid_in_nodes = __io.schema_validate_in_nodes(in_nodes);
      if (!type_valid_in_nodes) {
        return type_valid_in_nodes;
      }
      return plugin_validate_in_nodes(type_valid_in_nodes.value());
    }

    // Parameters
    /*
      Gets the default parameters from the plugin without validating in_nodes
    */
    required_plugin_type<std::expected<parameter_type, core::type_schema_validation_error>>
    plugin_get_default_parameters(const depends_type &in_nodes) const {
      using result_type = std::expected<parameter_type, core::type_schema_validation_error>;
      auto errors = core::type_schema_validation_error::make_empty();
      return plugin().get_default_parameters(in_nodes).transform([&](parameter_type &&params) {
        return __io.schema_validate_parameters(params);
      });
    }
    /*
      Gets the default parameters. This assumes that in_nodes is given, checks for in_nodes validity
      and returns the default parameters given the in_nodes
    */
    required_plugin_type<std::expected<parameter_type, core::type_schema_validation_error>>
    get_default_parameters(const depends_type &in_nodes) {
      using result_type = std::expected<parameter_type, core::type_schema_validation_error>;
      auto valid_in_nodes = validate_in_nodes(in_nodes);
      if (!valid_in_nodes) {
        return std::unexpected{std::move(valid_in_nodes.error())};
      } else if (!valid_in_nodes->has_value()) {
        return result_type{std::unexpected{std::move(valid_in_nodes->error())}};
      } else {
        return plugin_get_default_parameters(valid_in_nodes->value());
      }
    }
    /*
      Validates the parameters given to the manifest by passing it to the PLUGIN only. This should
      be called after type is validated. The behaviour if the parameter given is not validated by
      type yet is undefined.
    */
    required_plugin_type<std::expected<parameter_type, core::type_schema_validation_error>>
    plugin_validate_parameters(const parameter_type &parameters) const {
      using result_type = std::expected<parameter_type, core::type_schema_validation_error>;
      auto errors = core::type_schema_validation_error::make_empty();
      return plugin()
        .validate_parameters(parameters, errors)
        .transform([&](std::optional<std::optional<parameter_type>> &&params_res) {
          if (!params_res) {
            return result_type{parameters};
          } else if (!params_res->has_value()) {
            return result_type{std::unexpected{std::move(errors)}};
          } else {
            return __io.schema_validate_parameters(params_res->value());
          }
        });
    }
    /*
      validate the parameters to run the module. This will validate the parameter by type and pass
      it internally into the module manifest.
    */
    required_plugin_type<std::expected<parameter_type, core::type_schema_validation_error>>
    validate_parameters(const parameter_type &parameters) const {
      auto type_valid_res = __io.schema_validate_parameters(parameters);
      if (!type_valid_res) {
        return type_valid_res;
      } else {
        return plugin_validate_parameters(std::move(parameters));
      }
    }
    /*
      Get the amount of token required to run the module. This assumes that the parameters are
      valid.
    */
    required_plugin_type<int> get_required_token(const parameter_type &parameters) const {
      return plugin().get_required_token(parameters).transform([](auto &&opt) {
        return opt.value_or(0);
      });
    }
    /*
      gets the download options to download the module.
    */
    required_plugin_type<void> get_download_zip(
      const fs::path &zip_path, const fs::path &work_dir
    ) {
      using expected_type = required_plugin_type<void>;
      auto to_plugin_error = [](auto &&e) { return plugin_error{"get_download_zip", e.what()}; };
      return core::zip_builder::open(zip_path, work_dir)
        .transform_error(to_plugin_error)
        .and_then([&](auto &&builder) {
          return plugin().get_download_zip(builder, work_dir).and_then([&](auto &&opt_res) {
            if (opt_res) {
              return std::expected<core::zip_builder, plugin_error>{std::move(builder)};
            } else {
              return builder.add_directory(work_dir)
                .transform_error(to_plugin_error)
                .transform([&](core::zip_builder &builder) { return std::move(builder); });
            }
          });
        })
        .and_then([&](core::zip_builder &&builder) {
          return builder.finalize().transform_error(to_plugin_error);
        });
    }
  };

  /*
    Creates a plugin, this basically allows type inferrence to work.
  */
  export template <is_plugin plugin_type>
  std::expected<loaded_plugin<plugin_type>, plugin_error> make_plugin(plugin_type plugin) {
    return plugin_io::make(plugin).transform([&](plugin_io &&io) {
      return loaded_plugin<plugin_type>{std::move(plugin), std::move(io)};
    });
  }
}