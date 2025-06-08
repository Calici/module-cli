module;
#include <exception>
#include <format>
#include <string>
#include <string_view>
export module pharmaco.manifest:error;
namespace pharmaco::manifest {

  class plugin_error : public std::exception {
    std::string __msg;
    std::string_view __attr;
    std::string_view __reason;

  public:
    template <class... Args>
    plugin_error(std::string_view attr, std::format_string<Args...> fmt, Args &&...args) {
      auto msg_inserter = std::back_inserter(__msg);
      std::format_to(msg_inserter, "{}:", attr);
      std::format_to(msg_inserter, fmt, std::forward<Args>(args)...);
      __attr = std::string_view{__msg}.substr(0, attr.size());
      __reason = std::string_view{__msg.begin() + attr.size() + 2, __msg.end()};
    }
    plugin_error(std::string_view attr, std::string_view reason) :
      plugin_error(attr, "{}", reason) {}

    std::string_view attr() const noexcept {
      return __attr;
    }
    std::string_view reason() const noexcept {
      return __reason;
    }
    const char *what() const noexcept {
      return __msg.c_str();
    }
  };
}
