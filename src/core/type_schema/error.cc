module;
#include <format>
#include <string>
#include <string_view>
export module pharmaco.core.type_schema:error;
import moderna.generic;

namespace generic = moderna::generic;
namespace pharmaco::core {
  /*
    A type error is most likely related to programming error while a value type error is
    most likely user error
  */
  export enum struct type_schema_error_type { type, value };

  /*
    An error in the type schema
  */
  export struct type_schema_error {
    type_schema_error_type type;
    std::string error;

    static type_schema_error type_error(std::string err) {
      return type_schema_error{type_schema_error_type::type, err};
    }
    static type_schema_error value_error(std::string err) {
      return type_schema_error{type_schema_error_type::value, err};
    }
  };
  export class type_schema_validation_error {
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
}

namespace core = pharmaco::core;
template <class char_type> struct std::formatter<core::type_schema_error, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const core::type_schema_error &e, auto &ctx) const {
    if (e.type == core::type_schema_error_type::type) {
      return std::format_to(ctx.out(), "TypeError[{}]", e.error);
    } else {
      return std::format_to(ctx.out(), "ValueError[{}]", e.error);
    }
  }
};
template <class char_type> struct std::formatter<core::type_schema_validation_error, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const core::type_schema_validation_error &errs, auto &ctx) const {
    if (errs.empty()) {
      std::format_to(ctx.out(), "type_schema_validation_error[]");
    } else {
      std::format_to(ctx.out(), "type_schema_validation_error[\n");
      for (const auto &[name, error] : errs) {
        std::format_to(ctx.out(), "  {}: {}\n", name, error);
      }
      std::format_to(ctx.out(), "]");
    }
    return ctx.out();
  }
};