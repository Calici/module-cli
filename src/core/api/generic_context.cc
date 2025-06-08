module;
#include <charconv>
#include <expected>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
export module pharmaco.core.api:generic_context;
import pharmaco.core.curl;
import moderna.generic;
import moderna.process;
import :context_error;

namespace pharmaco::core::api {
  namespace proc = moderna::process;
  export struct empty_extra_context {};
  export struct empty_extra_context_loader {
    std::expected<empty_extra_context, context_error> operator()(
      const proc::subprocess_env &
    ) const {
      return empty_extra_context{};
    }
  };
  export template <generic::static_string env_name> struct named_env_loader {
    std::expected<std::string_view, context_error> operator()(
      const proc::subprocess_env &env
    ) const {
      using result_type = std::expected<std::string_view, context_error>;
      return env.get(env_name.as_view())
        .transform([](proc::env_entry &&entry) { return result_type{entry.value}; })
        .or_else([]() {
          return std::optional{
            result_type{std::unexpected{context_error{env_name.as_view(), "missing"}}}
          };
        })
        .value();
    }
  };

  export template <
    generic::static_string base_url_env_name,
    generic::static_string auth_token_env_name,
    class extra_context = empty_extra_context,
    std::invocable<const proc::subprocess_env &> F = empty_extra_context_loader>
    requires(std::is_invocable_r_v<
             std::expected<extra_context, context_error>,
             F,
             const proc::subprocess_env &>)
  class generic_context {

    static std::optional<generic_context> __ctx;

  public:
    std::string base_url;
    std::string auth_token;
    extra_context extras;

    /*
      Formats a suburl into a full url by appending the base url stored in this current cotnext
    */
    template <class... Args>
    std::string get_url(
      std::format_string<const std::string &, Args...> fmt, Args &&...args
    ) const {
      return std::format(fmt, base_url, std::forward<Args>(args)...);
    }

    /*
      Returns the authentication header used
    */
    std::string get_auth_header() const {
      return std::format("Authorization: Bearer {}", auth_token);
    }

    /*
      Now factory functions
    */
    template <class response_type, class data_type>
    std::expected<curl_json_response<response_type>, curl_http_error> make_request(
      std::string_view url, curl_http_request_method method, const data_type &data
    ) const {
      return curl_http_request::make_json_request(url, method, data)
        .and_then([&](curl_http_request &&request) {
          request.add_header(get_auth_header());
          return request.send_json<response_type>();
        });
    }

    template <class response_type>
    std::expected<curl_json_response<response_type>, curl_http_error> make_request(
      std::string_view url, curl_http_request_method method
    ) const {
      return curl_http_request::make_json_request(url, method)
        .and_then([&](curl_http_request &&request) {
          request.add_header(get_auth_header());
          return request.send_json<response_type>();
        });
    }

    static std::expected<generic_context, context_error> load_from_env(
      F extra_loader, const proc::subprocess_env &env = proc::subprocess_env::global_env()
    ) {
      std::expected<generic_context, context_error> res{std::unexpected{context_error{"", ""}}};
      auto base_url_res =
        named_env_loader<base_url_env_name>{}(env).and_then([](std::string_view url) {
          using result_type = std::expected<std::string, context_error>;
          if (url.size() == 0) {
            return result_type{
              std::unexpected{context_error{base_url_env_name.as_view(), "missing"}}
            };
          } else if (url.ends_with("/")) {
            return result_type{std::string{url.begin(), url.begin() + url.size() - 1}};
          } else {
            return result_type{std::string{url}};
          }
        });
      if (!base_url_res) {
        res = std::unexpected{std::move(base_url_res.error())};
        return res;
      }
      auto auth_token_res =
        named_env_loader<auth_token_env_name>{}(env).transform([](std::string_view url) {
          return std::string{url};
        });
      if (!auth_token_res) {
        res = std::unexpected{std::move(auth_token_res.error())};
        return res;
      }
      auto extra_context_res = extra_loader(env);
      if (!extra_context_res) {
        res = std::unexpected{std::move(extra_context_res.error())};
        return res;
      }
      res = generic_context{
        .base_url = std::move(base_url_res.value()),
        .auth_token = std::move(auth_token_res.value()),
        .extras = std::move(extra_context_res.value())
      };
      return res;
    }
    static std::expected<generic_context, context_error> load_from_env(
      const proc::subprocess_env &env = proc::subprocess_env::global_env()
    )
      requires(std::is_default_constructible_v<F>)
    {
      return load_from_env(F{}, env);
    }
    static const generic_context &get_global_context()
      requires(std::is_default_constructible_v<F>)
    {
      if (!__ctx) {
        auto loaded = load_from_env();
        if (!loaded) {
          throw loaded.error();
        }
        __ctx = std::move(loaded.value());
      }
      return __ctx.value();
    }
  };

  struct id_extra_context {
    size_t id;
  };
  export template <generic::static_string env_name> struct id_env_loader {
    std::expected<id_extra_context, context_error> operator()(
      const proc::subprocess_env &env
    ) const {
      using result_type = std::expected<id_extra_context, context_error>;
      return named_env_loader<env_name>{}(env).and_then([](std::string_view &&entry) {
        size_t num;
        auto res = std::from_chars(entry.begin(), entry.end(), num);
        if (res.ec == std::errc{}) {
          return result_type{id_extra_context{num}};
        } else {
          return result_type{std::unexpected{context_error{env_name.as_view(), "cannot parse"}}};
        }
      });
    }
  };

  export using developer_context = generic_context<
    generic::make_static_string("PHARMACO_NET_BASE_URL"),
    generic::make_static_string("PHARMACO_NET_AUTH_TOKEN")>;

  export using module_context = generic_context<
    generic::make_static_string("PHARMACO_NET_BASE_URL"),
    generic::make_static_string("PHARMACO_NET_AUTH_TOKEN"),
    id_extra_context,
    id_env_loader<generic::make_static_string("PHARMACO_NET_MODULE_ID")>>;

  template <> std::optional<developer_context> developer_context::__ctx = std::nullopt;
  template <> std::optional<module_context> module_context::__ctx = std::nullopt;
}