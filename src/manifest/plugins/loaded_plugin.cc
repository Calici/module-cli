module;
#include <rfl/Generic.hpp>
#include <algorithm>
#include <expected>
#include <filesystem>
export module pharmaco.manifest:loaded_plugin;
import :is_plugin;
import :zip_builder;
import :error;

namespace pharmaco::manifest {
  export struct plugin_io {
    typed_record_type_schema params;
    typed_record_type_schema results;
    io_nodes in_nodes;
    io_nodes out_nodes;
  };

  export template <class param_type> struct parameter_validation_result {
    param_type parameters;
    int required_token;
    int run_count;
  };

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
    const typed_record_type_schema &params() const noexcept {
      return __io.params;
    }
    const typed_record_type_schema &results() const noexcept {
      return __io.results;
    }
    const io_nodes &in_nodes() const noexcept {
      return __io.in_nodes;
    }
    const io_nodes &out_nodes() const noexcept {
      return __io.out_nodes;
    }
    required_plugin_type<parameter_type> get_default_parameters() const {
      return __plugin.get_default_parameters();
    }

    /*
      Optional Arguments
    */

    /*
      Validates the parameters given to the manifest by type only. This will indicate if the
      parameters satisfy the type given.
    */
    std::expected<parameter_type, type_schema_validation_error> type_validate_parameters(
      const parameter_type &parameters
    ) const {
      using expected_type = std::expected<parameter_type, type_schema_validation_error>;
      auto errors = type_schema_validation_error::make_empty();
      return params()
        .validate_parameters(parameters, errors, "")
        .transform([](auto &&params) { return expected_type{std::move(params)}; })
        .value_or(expected_type{std::unexpected{std::move(errors)}});
    }

    /*
      Validates the parameters given to the manifest by passing it to the PLUGIN only. This should
      be called after type is validated. The behaviour if the parameter given is not validated by
      type yet is undefined.
    */
    required_plugin_type<std::expected<parameter_type, type_schema_validation_error>>
    plugin_validate_parameters(const parameter_type &parameters) const {
      return plugin().validate_parameters(parameters).transform([&](auto &&params_res) {
        return params_res
          .transform([&](auto &&valid_params_res) {
            return valid_params_res.and_then([&](auto &&valid_params) {
              // Type validate.
              return type_validate_parameters(std::move(valid_params));
            });
          })
          .or_else([&]() {
            // No need to type validate.
            return std::optional{
              std::expected<parameter_type, type_schema_validation_error>{parameters}
            };
          })
          .value();
      });
    }

    /*
      Get the amount of token required to run the module.
    */
    required_plugin_type<int> get_required_token(const parameter_type &parameters) const {
      return plugin().get_required_token(parameters).transform([](auto &&opt) {
        return opt.value_or(0);
      });
    }

    /*
      validate the parameters to run the module. This will validate the parameter by type and pass
      it internally into the module manifest.
    */
    required_plugin_type<std::expected<parameter_type, type_schema_validation_error>>
    validate_parameters(const parameter_type &parameters) const {
      using expected_type = std::expected<parameter_type, type_schema_validation_error>;
      auto type_valid_res = type_validate_parameters(parameters);
      if (type_valid_res) {
        return plugin_validate_parameters(type_valid_res.value());
      } else {
        return expected_type{std::unexpected{std::move(type_valid_res.error())}};
      }
    }

    /*
      gets the download options to download the module.
    */
    required_plugin_type<void> get_download_zip(
      const fs::path &zip_path, const fs::path &work_dir
    ) {
      using expected_type = required_plugin_type<void>;
      auto to_plugin_error = [](auto &&e) { return plugin_error{"get_download_zip", e.what()}; };
      return zip_builder::open(zip_path, work_dir)
        .transform_error(to_plugin_error)
        .and_then([&](auto &&builder) {
          return plugin().get_download_zip(builder, work_dir).and_then([&](auto &&opt_res) {
            if (opt_res) {
              return std::expected<zip_builder, plugin_error>{std::move(builder)};
            } else {
              return builder.add_directory(work_dir)
                .transform_error(to_plugin_error)
                .transform([&](zip_builder &builder) { return std::move(builder); });
            }
          });
        })
        .and_then([&](zip_builder &&builder) {
          return builder.finalize().transform_error(to_plugin_error);
        });
    }
  };

  /*
    Creates a plugin, this basically allows type inferrence to work.
  */
  export template <is_plugin plugin_type>
  std::expected<loaded_plugin<plugin_type>, plugin_error> make_plugin(plugin_type plugin) {
    return plugin.get_params_schema().and_then([&](auto &&params) {
      return plugin.get_results_schema().and_then([&](auto &&results) {
        return plugin.get_in_nodes().and_then([&](auto &&in_nodes) {
          return plugin.get_out_nodes().transform([&](auto &&out_nodes) {
            return loaded_plugin{
              std::move(plugin),
              plugin_io{
                .params{std::move(params)},
                .results{std::move(results)},
                .in_nodes{std::move(in_nodes)},
                .out_nodes{std::move(out_nodes)}
              }
            };
          });
        });
      });
    });
  }
}