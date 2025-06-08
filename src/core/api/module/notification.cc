module;
#include <expected>
#include <string>
export module pharmaco.core.api:notification;
import pharmaco.core.curl;
import :generic_context;

namespace pharmaco::core::api {
  export enum struct notification_type { COMPLETE, ERROR, GENERAL, WARNING };
  export struct notification_create_data {
    std::string_view title;
    std::string_view content;
    size_t module_id;
    notification_type type;
  };
  export struct notification {
    static std::expected<curl_json_response<std::string>, curl_http_error> create(
      std::string_view title,
      std::string_view content,
      notification_type type = notification_type::GENERAL,
      const module_context &ctx = module_context::get_global_context()
    ) {
      return ctx.make_request<std::string>(
        ctx.get_url("{}/notification"),
        curl_http_request_method::POST,
        notification_create_data{title, content, ctx.extras.id, type}
      );
    }
  };
}
