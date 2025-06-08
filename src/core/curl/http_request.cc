module;
#include <curl/curl.h>
#include <rfl/json.hpp>
#include <concepts>
#include <expected>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
export module pharmaco.core.curl:http_request;
import :error;
import :http_response;
import :basic_request;
import :cleaner;

namespace pharmaco::core::curl {

  export enum struct http_request_method { GET, POST, PUT, PATCH, DELETE };

  export class http_request {
    basic_request __req;
    curl_list_pointer __headers;
    std::optional<std::string> __data;

  public:
    http_request(basic_request req, std::optional<std::string> data = std::nullopt) :
      __req{std::move(req)}, __data{std::move(data)} {}

    template <is_easy_setopt_arg arg_type>
    std::expected<void, http_error> set_option(CURLoption option, arg_type &&arg) {
      return __req.set_option(option, std::forward<arg_type>(arg))
        .transform_error(http_error::from_curl_error);
    }

    std::expected<void, http_error> set_url(std::string_view url) {
      return set_option(CURLoption::CURLOPT_URL, url.data());
    }
    std::expected<void, http_error> set_method(http_request_method method) {
      switch (method) {
        case http_request_method::GET:
          return set_option(CURLoption::CURLOPT_CUSTOMREQUEST, "GET");
        case http_request_method::POST:
          return set_option(CURLoption::CURLOPT_CUSTOMREQUEST, "POST");
        case http_request_method::PUT:
          return set_option(CURLoption::CURLOPT_CUSTOMREQUEST, "PUT");
        case http_request_method::PATCH:
          return set_option(CURLoption::CURLOPT_CUSTOMREQUEST, "PATCH");
        case http_request_method::DELETE:
          return set_option(CURLoption::CURLOPT_CUSTOMREQUEST, "DELETE");
      }
    }
    http_request &add_header(std::string_view header) {
      auto *new_list = curl_slist_append(__headers.get(), header.data());
      if (new_list == nullptr) {
        throw std::bad_alloc{};
      }
      auto *old_list = __headers.release();
      __headers.reset(new_list);
      return *this;
    }
    std::expected<void, http_error> follow_redirects(bool follow) {
      return set_option(CURLoption::CURLOPT_FOLLOWLOCATION, follow);
    }
    std::expected<http_response, http_error> send() {
      return set_option(CURLoption::CURLOPT_HEADER, __headers.get()).and_then([&]() {
        return http_response::get_response(__req)
          .transform_error(http_error::from_curl_error)
          .and_then(http_error::separate_response);
      });
    }
    template <class response_data>
    std::expected<json_response<response_data>, http_error> send_json() {
      return send().and_then(json_response<response_data>::from_response);
    }

    std::expected<void, http_error> set_data(std::string_view data) {
      __data = std::string{data};
      return set_option(CURLoption::CURLOPT_POSTFIELDS, __data->c_str());
    }

    static std::expected<http_request, http_error> make_request(
      std::string_view url, http_request_method method, bool follow_redirect = true
    ) {
      return basic_request::make_request()
        .transform_error(http_error::from_curl_error)
        .transform([](auto &&basic_req) { return http_request{std::move(basic_req)}; })
        .and_then([&](auto &&http_req) {
          return http_req.set_method(method)
            .and_then([&]() { return http_req.set_url(url); })
            .and_then([&]() { return http_req.follow_redirects(follow_redirect); })
            .transform([&]() { return std::move(http_req); });
        });
    }

    template <class request_data>
    static std::expected<http_request, http_error> make_json_request(
      std::string_view url,
      http_request_method method,
      const request_data &data,
      bool follow_redirect = true
    ) {
      return make_request(url, method, follow_redirect).and_then([&](auto &&request) {
        return request.set_data(rfl::json::write(data)).transform([&]() {
          request.add_header("Content-Type: application/json");
          return std::move(request);
        });
      });
    }
    static std::expected<http_request, http_error> make_json_request(
      std::string_view url, http_request_method method, bool follow_redirect = true
    ) {
      return make_request(url, method, follow_redirect).transform([&](auto &&request) {
        request.add_header("Content-Type: application/json");
        return std::move(request);
      });
    }
  };
}

namespace pharmaco::core {
  export using curl_http_request = curl::http_request;
  export using curl_http_request_method = curl::http_request_method;
}