module;
#include <expected>
#include <rfl.hpp>
#include <string>
export module pharmaco.core.api:plugin_module;
import pharmaco.core.curl;
import :generic_context;
import :utils;

namespace pharmaco::core::api {
  struct plugin_module_data {
    // We only need the field over here.
    std::string name;
    std::string internal_name;
    size_t id;

    // These are for extra fields returned from the API that will be unparsed;
    rfl::ExtraFields<rfl::Generic> extras;
  };

  struct plugin_module_update_data {
    std::optional<std::string> name;
    std::optional<std::string> internal_name;
  };

  class plugin_module {
    plugin_module_data __data;
    developer_context __ctx;

  public:
    plugin_module(plugin_module_data data, developer_context ctx) :
      __data{std::move(data)}, __ctx{std::move(ctx)} {}

    std::expected<void, curl_http_error> rename(std::string_view new_name) {
      return __ctx
        .make_request<plugin_module_data>(
          __ctx.get_url("{}/develop/plugin_module/{}", __data.id),
          curl_http_request_method::POST,
          plugin_module_update_data{.name{new_name}, .internal_name{std::nullopt}}
        )
        .transform([this](auto &&resp) { __data = std::move(resp.data); });
    }

    static std::expected<std::optional<plugin_module>, curl_http_error> from_internal_name(
      std::string_view internal_name,
      const developer_context &ctx = developer_context::get_global_context()
    ) {
      return ctx
        .make_request<std::vector<plugin_module_data>>(
          ctx.get_url("{}/develop/plugin_module?internal_name={}", internal_name),
          curl_http_request_method::GET
        )
        .transform([](auto &&resp) { return std::move(resp.data); })
        .transform(entry_coercer{})
        .transform([&](auto &&opt_base_module) {
          return opt_base_module.transform([&](auto &&data) {
            return plugin_module{std::move(data), ctx};
          });
        });
    }
    static std::expected<std::optional<plugin_module>, curl_http_error> from_name(
      std::string_view name, const developer_context &ctx = developer_context::get_global_context()
    ) {
      return ctx
        .make_request<std::vector<plugin_module_data>>(
          ctx.get_url("{}/develop/plugin_module?name={}", name), curl_http_request_method::GET
        )
        .transform([](auto &&resp) { return std::move(resp.data); })
        .transform(entry_coercer{})
        .transform([&](auto &&opt_base_module) {
          return opt_base_module.transform([&](auto &&data) {
            return plugin_module{std::move(data), ctx};
          });
        });
    }
  };
}