module;
#include <exception>
#include <format>
#include <string>
#include <string_view>
export module pharmaco.core.api:context_error;
import moderna.generic;

namespace generic = moderna::generic;
namespace pharmaco::core::api {
  /*
    context error defines when there is a problem loading the context from the environment.
  */
  export class context_error : public std::exception {
    std::string __msg;
    std::string_view __attr;

  public:
    context_error(std::string_view attr, const std::formattable<char> auto &msg) :
      __msg{std::format("{}: {}", attr, msg)},
      __attr{std::string_view{__msg}.substr(0, attr.size())} {}
    context_error(std::string_view attr, const generic::is_formattable_error auto &e) :
      context_error(attr, generic::error_formatter{e}) {}

    std::string_view attr() const noexcept {
      return __attr;
    }

    const char *what() const noexcept {
      return __msg.c_str();
    }
  };
}