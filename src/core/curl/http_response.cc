module;
#include <curl/curl.h>
#include <rfl/json.hpp>
#include <algorithm>
#include <iterator>
#include <string>
export module pharmaco.core.curl:http_response;
import moderna.generic;
import :error;
import :basic_request;

namespace generic = moderna::generic;
namespace pharmaco::core::curl {
  export struct http_response {
    long status_code;
    std::string body;

    static std::expected<http_response, curl_error> get_response(basic_request &request) {
      http_response response;
      auto inserter = std::back_inserter(response.body);
      return request.set_option(CURLoption::CURLOPT_WRITEDATA, static_cast<void *>(&inserter))
        .and_then([&]() {
          return request.set_option(CURLoption::CURLOPT_WRITEFUNCTION, request_callback);
        })
        .and_then([&]() { return request.perform(); })
        .and_then([&]() {
          return request.get_info(CURLINFO::CURLINFO_RESPONSE_CODE, &response.status_code);
        })
        .transform([&](auto &&) { return std::move(response); });
    }
    static size_t request_callback(const char *ptr, size_t size, size_t nmemb, void *userdata) {
      auto inserter = *static_cast<std::back_insert_iterator<std::string> *>(userdata);
      std::ranges::copy_n(ptr, nmemb, inserter);
      return nmemb;
    }
  };
  export struct http_error : public std::exception {
    using variant_type = generic::variant<http_response, curl_error>;
    generic::variant<http_response, curl_error> error;

    template <class... Args>
      requires(std::is_constructible_v<variant_type, Args...>)
    http_error(Args &&...args) : error{std::forward<Args>(args)...} {}

    bool is_http() const {
      return error.is<http_response>();
    }
    bool is_curl() const {
      return error.is<curl_error>();
    }
    const char *what() const noexcept {
      return error.visit([](const auto &v) {
        using value_type = std::decay_t<decltype(v)>;
        if constexpr (std::same_as<value_type, http_response>) {
          return v.body.c_str();
        } else {
          return v.what();
        }
      });
    }
    std::optional<CURLcode> curl_code() const noexcept {
      return error.as<curl_error>().and_then([](const curl_error &error) { return error.code(); });
    }
    std::optional<long> status_code() const noexcept {
      return error.as<http_response>().transform([](const http_response &response) {
        return response.status_code;
      });
    }

    static http_error from_curl_error(curl_error err) {
      return http_error{std::move(err)};
    }
    static std::expected<http_response, http_error> separate_response(http_response resp) {
      if (resp.status_code >= 200 && resp.status_code < 300) {
        return std::move(resp);
      } else {
        return std::unexpected{http_error{std::move(resp)}};
      }
    }
  };
  export template <class T> struct json_response {
    long status_code;
    T data;

    static std::expected<json_response, http_error> from_response(http_response resp) {
      using expected_type = std::expected<json_response, http_error>;
      return http_error::separate_response(resp).and_then([](http_response &&resp) {
        if constexpr (std::same_as<T, std::string>) {
          return expected_type{json_response{resp.status_code, std::move(resp.body)}};
        } else {
          return rfl::json::read<T>(resp.body)
            .transform([&](T &&data) { return json_response{resp.status_code, std::move(data)}; })
            .transform_error([&](auto&& ) { return http_error{std::move(resp)}; });
        }
      });
    };
  };
}
namespace pharmaco::core {
  export using curl_http_response = curl::http_response;
  export using curl_http_error = curl::http_error;
  export template <class T> using curl_json_response = curl::json_response<T>;
}