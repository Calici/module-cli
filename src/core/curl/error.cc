module;
#include <curl/curl.h>
#include <exception>
#include <format>
#include <optional>
#include <string>
export module pharmaco.core.curl:error;
import moderna.generic;

namespace generic = moderna::generic;
namespace pharmaco::core {
  export class curl_error : public std::exception {
    std::optional<CURLcode> __code;
    std::string __msg;

  public:
    curl_error(const generic::is_formattable_error auto &e) :
      __msg{std::format("{}", generic::error_formatter{e})} {}
    curl_error(CURLcode code) : __code{code}, __msg{curl_easy_strerror(code)} {}
    curl_error(std::string msg) : __msg{std::move(msg)} {}

    const char *what() const noexcept {
      return __msg.c_str();
    }

    std::optional<CURLcode> code() const {
      return __code;
    }
  };
}