module;
#include <curl/curl.h>
#include <concepts>
#include <expected>
#include <utility>
export module pharmaco.core.curl:basic_request;
import :error;
import :cleaner;

namespace pharmaco::core::curl {
  export template <class Arg>
  concept is_easy_setopt_arg = requires(Arg &&arg, CURL *curl, CURLoption option) {
    { curl_easy_setopt(curl, option, std::forward<Arg>(arg)) };
  };
  export template <class Arg>
  concept is_easy_getinfo_arg = requires(Arg &&arg, CURL *curl, CURLINFO option) {
    { curl_easy_getinfo(curl, option, std::forward<Arg>(arg)) };
  };

  template <class F, class... Args>
    requires(std::is_invocable_r_v<CURLcode, F, Args...>)
  std::expected<void, curl_error> perform_curl_call(F &&f, Args &&...args) {
    CURLcode code = std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    if (code == CURLcode::CURLE_OK) {
      return {};
    } else {
      return std::unexpected{curl_error{code}};
    }
  }

  export class basic_request {
    curl_pointer __curl;

  public:
    basic_request(curl_pointer curl) : __curl{std::move(curl)} {}

    /*
      sets a curl option
    */
    template <is_easy_setopt_arg Arg>
    std::expected<void, curl_error> set_option(CURLoption option, Arg &&arg) {
      return perform_curl_call(curl_easy_setopt, __curl.get(), option, std::forward<Arg>(arg));
    }

    /*
      get curl info
    */
    template <is_easy_getinfo_arg Arg>
    std::expected<Arg, curl_error> get_info(CURLINFO option, Arg &&arg) {
      return perform_curl_call(curl_easy_getinfo, __curl.get(), option, std::forward<Arg>(arg))
        .transform([&]() { return std::move(arg); });
    }

    /*
      perform
    */
    std::expected<void, curl_error> perform() {
      return perform_curl_call(curl_easy_perform, __curl.get());
    }

    static std::expected<void, curl_error> init_global() {
      return perform_curl_call(curl_global_init, CURL_GLOBAL_ALL);
    }

    static std::expected<basic_request, curl_error> make_request() {
      CURL *raw_curl = curl_easy_init();
      if (raw_curl == nullptr) {
        return std::unexpected{curl_error{CURLcode::CURLE_FAILED_INIT}};
      }
      return basic_request{curl_pointer{raw_curl}};
    }
  };
};

namespace pharmaco::core {
  export using curl_basic_request = curl::basic_request;
}