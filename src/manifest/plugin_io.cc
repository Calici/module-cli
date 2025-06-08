module;
#include <expected>
#include <format>
export module pharmaco.manifest:plugin_io;
import pharmaco.core.type_schema;
import pharmaco.core.io_nodes;
import :is_plugin;
import :error;

namespace core = pharmaco::core;
namespace pharmaco::manifest {
  export struct plugin_io {
    core::typed_record_type_schema params_schema;
    core::typed_record_type_schema results_schema;
    core::typed_record_type_schema in_nodes_schema;
    core::io_nodes in_nodes;
    core::io_nodes out_nodes;

    std::expected<parameter_type, core::type_schema_validation_error> schema_validate_parameters(
      const parameter_type &params
    ) const {
      return params_schema.validate_parameters(params);
    }
    std::expected<result_type, core::type_schema_validation_error> schema_validate_results(
      const result_type &results
    ) const {
      return results_schema.validate_parameters(results);
    }
    std::expected<depends_type, core::type_schema_validation_error> schema_validate_in_nodes(
      const depends_type &in_nodes
    ) const {
      return in_nodes_schema.validate_parameters(in_nodes);
    }

    template <is_plugin plugin_type>
    static std::expected<plugin_io, plugin_error> make(const plugin_type &plugin) {
      using result_type = std::expected<plugin_io, plugin_error>;
      return plugin.get_params_schema().and_then([&](core::typed_record_type_schema &&params) {
        return plugin.get_in_nodes_schema().and_then(
          [&](core::typed_record_type_schema &&in_nodes_schema) {
            return plugin.get_in_nodes().and_then([&](core::io_nodes &&in_nodes) {
              for (const auto &[name, _] : in_nodes) {
                auto schema_node = in_nodes_schema.child.get(name);
                if (!schema_node) {
                  return result_type{
                    std::unexpected{plugin_error{"schema_node", "{} does not exist", name}}
                  };
                }
              }
              return plugin.get_out_nodes().and_then([&](core::io_nodes &&out_nodes) {
                return plugin.get_results_schema().and_then(
                  [&](core::typed_record_type_schema &&results) {
                    for (const auto &[name, _] : out_nodes) {
                      auto result_node = results.child.get(name);
                      if (!result_node) {
                        return result_type{
                          std::unexpected{plugin_error{"results_schema", "{} does not exist", name}}
                        };
                      }
                    }
                    return result_type{plugin_io{
                      std::move(params),
                      std::move(results),
                      std::move(in_nodes_schema),
                      std::move(in_nodes),
                      std::move(out_nodes)
                    }};
                  }
                );
              });
            });
          }
        );
      });
    }
  };
}