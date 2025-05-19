module;
#include <exception>
#include <format>
#include <string>
#include <string_view>
export module pharmaco.manifest:error;
import moderna.generic;

namespace generic = moderna::generic;
namespace pharmaco::manifest {
  class error : public std::exception {
    std::string __msg;
    std::string __path;
    std::string __full_msg;

  public:
    error(std::string msg, std::string path) :
      __msg{std::move(msg)}, __path{std::move(path)},
      __full_msg{std::format("At {} found {}", __path, __msg)} {}
    std::string_view path() const noexcept {
      return __path;
    }
    std::string_view msg() const noexcept {
      return __msg;
    }
    const char *what() const noexcept {
      return __full_msg.c_str();
    }
    void prepend_path(std::string_view path) noexcept {
      __path.insert(0, ".");
      __path.insert(0, path);
      __full_msg = std::format("At {} found {}", __path, __msg);
    }
  };

  /*
    A type error is most likely related to programming error while a value type error is
    most likely user error
  */
  enum struct type_schema_error_type { type, value };

  /*
    An error in the type schema
  */
  struct type_schema_error {
    type_schema_error_type type;
    std::string error;

    static type_schema_error type_error(std::string err) {
      return type_schema_error{type_schema_error_type::type, err};
    }
    static type_schema_error value_error(std::string err) {
      return type_schema_error{type_schema_error_type::value, err};
    }
  };
  class type_schema_validation_error {
    generic::key_vector<std::string, type_schema_error> __errors;

  public:
    type_schema_validation_error() {}
    auto get(std::string_view name) const noexcept {
      return __errors.get(name);
    }
    type_schema_validation_error &insert(std::string_view name, type_schema_error err) {
      __errors.insert(std::string{name}, std::move(err));
      return *this;
    }
    type_schema_validation_error &insert_type_error(std::string_view name, std::string err) {
      return insert(std::string{name}, type_schema_error::type_error(std::move(err)));
    }
    type_schema_validation_error &insert_value_error(std::string_view name, std::string err) {
      __errors.insert(std::string{name}, type_schema_error::value_error(std::move(err)));
      return *this;
    }
    auto begin() const noexcept {
      return __errors.begin();
    }
    auto end() const noexcept {
      return __errors.end();
    }
    auto size() const noexcept {
      return __errors.size();
    }
    auto empty() const noexcept {
      return __errors.empty();
    }

    static type_schema_validation_error make_empty() {
      return type_schema_validation_error{};
    }
  };
  class plugin_error {
    std::string __msg;
    std::string_view __attr;
    std::string_view __reason;

  public:
    plugin_error(std::string_view attr, std::string_view reason) :
      __msg{std::format("{}: {}", attr, reason)} {
      __attr = std::string_view{__msg}.substr(0, attr.size());
      __reason = std::string_view{__msg.begin() + attr.size() + 2, __msg.end()};
    }
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

namespace manifest = pharmaco::manifest;

template <class char_type> struct std::formatter<manifest::type_schema_error, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const manifest::type_schema_error &e, auto &ctx) const {
    if (e.type == manifest::type_schema_error_type::type) {
      return std::format_to(ctx.out(), "TypeError[{}]", e.error);
    } else {
      return std::format_to(ctx.out(), "ValueError[{}]", e.error);
    }
  }
};

template <class char_type>
struct std::formatter<manifest::type_schema_validation_error, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const manifest::type_schema_validation_error &errs, auto &ctx) const {
    if (errs.empty()) {
      std::format_to(ctx.out(), "type_schema_validation_error[]");
    } else {
      std::format_to(ctx.out(), "type_schema_validation_error");
      for (const auto &[name, error] : errs) {
        std::format_to(ctx.out(), "  {}: {}\n", name, error);
      }
    }
    return ctx.out();
  }
};