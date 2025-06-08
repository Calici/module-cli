module;
#include <expected>
#include <format>
#include <string>
export module pharmaco.core.api:token;
import pharmaco.core.curl;
import :generic_context;

namespace pharmaco::core::api::token {
  export struct use_payload {
    int amount;
  };
  export std::expected<curl_json_response<std::string>, curl_http_error> use(
    int amount, const module_context &ctx = module_context::get_global_context()
  ) {
    return ctx.make_request<std::string>(
      ctx.get_url("{}/token.use"), curl_http_request_method::POST, use_payload{amount}
    );
  }

  export struct set_payload {
    int amount;
  };
  export std::expected<curl_json_response<std::string>, curl_http_error> set(
    int amount, const module_context &ctx = module_context::get_global_context()
  ) {
    return ctx.make_request<std::string>(
      ctx.get_url("{}/token.set"), curl_http_request_method::POST, set_payload{amount}
    );
  }

  export std::expected<curl_json_response<std::string>, curl_http_error> use_all(
    const module_context &ctx = module_context::get_global_context()
  ) {
    return ctx.make_request<std::string>(
      ctx.get_url("{}/token.use_all"), curl_http_request_method::POST
    );
  }
}