module;
#include <rfl/json.hpp>
#include <rfl/yaml.hpp>
#include <format>
#include <memory>
#include <rfl.hpp>
#include <string>
#include <string_view>
export module pharmaco.core.type_schema:type_schema;
import moderna.generic;
import :error;

namespace generic = moderna::generic;
namespace pharmaco::core {
  export struct type_schema {
    struct basic {
      rfl::Literal<"none", "int", "float", "bool", "str"> type;

      std::optional<rfl::Generic> validate_parameters(
        const rfl::Generic &v,
        type_schema_validation_error &errors,
        std::string_view current_path = ""
      ) const {
        return std::visit(
          [&](const auto &v) -> std::optional<rfl::Generic> {
            using value_type = std::decay_t<decltype(v)>;
            auto cur_id = type.value();
            if constexpr (std::same_as<value_type, std::nullopt_t>) {
              if (cur_id == 0) {
                return rfl::Generic{v};
              }
            } else if constexpr (std::same_as<value_type, int64_t>) {
              if (cur_id == 1) {
                return rfl::Generic{v};
              }
            } else if constexpr (std::same_as<value_type, double>) {
              if (cur_id == 2) {
                return rfl::Generic{v};
              }
            } else if constexpr (std::same_as<value_type, bool>) {
              if (cur_id == 3) {
                return rfl::Generic{v};
              }
            } else if constexpr (std::same_as<value_type, std::string>) {
              if (cur_id == 4) {
                return rfl::Generic{v};
              }
            }
            errors.insert_type_error(current_path, std::format("Expected {}", type.name()));
            return std::nullopt;
          },
          v.variant()
        );
      }

      friend bool operator==(const basic &l, const basic &r) {
        return l.type.value() == r.type.value();
      }

      static basic make(const std::string &t) {
        return basic{.type{t}};
      }
    };
    struct literal {
      rfl::Literal<"literal"> type;
      std::vector<std::string> literals;

      literal &add(std::string_view literal) {
        auto it = std::ranges::find(literals, literal);
        if (it == literals.end()) {
          literals.emplace_back(literal);
        }
        return *this;
      }

      std::optional<rfl::Generic> validate_parameters(
        const rfl::Generic &v,
        type_schema_validation_error &errors,
        std::string_view current_path = ""
      ) const {
        return std::visit(
          [&](const auto &v) -> std::optional<rfl::Generic> {
            using value_type = std::decay_t<decltype(v)>;
            if constexpr (std::same_as<value_type, std::string>) {
              for (const auto &lit : literals) {
                if (v == lit) {
                  return rfl::Generic{v};
                }
              }
            }
            errors.insert_type_error(current_path, "invalid literal");
            return std::nullopt;
          },
          v.variant()
        );
      }

      friend bool operator==(const literal &l, const literal &r) {
        if (l.literals.size() != r.literals.size()) {
          return false;
        }
        for (const auto &l_l : l.literals) {
          auto it = std::ranges::find(r.literals, l_l);
          if (it == r.literals.end()) {
            return false;
          }
        }
        return true;
      }

      static literal make() {
        return literal{.type{"literal"}, .literals{}};
      }
      template <class... Args>
        requires(std::is_constructible_v<std::string_view, Args> && ...)
      static literal make(Args &&...args) {
        auto schema = make();
        (schema.add(args), ...);
        return schema;
      }
    };
    struct list {
      rfl::Literal<"list"> type;
      std::unique_ptr<type_schema> child;

      list clone() const {
        return list{.type{type}, .child{std::make_unique<type_schema>(child->clone())}};
      }

      std::optional<rfl::Generic::Array> validate_parameters(
        const rfl::Generic &v,
        type_schema_validation_error &errors,
        std::string_view current_path = ""
      ) const {
        return std::visit(
          [&](const auto &v) -> std::optional<rfl::Generic::Array> {
            using value_type = std::decay_t<decltype(v)>;
            if constexpr (std::same_as<value_type, rfl::Generic::Array>) {
              rfl::Generic::Array arr;
              size_t pre_error_size = errors.size();
              for (const auto &entry : v) {
                auto res = child->validate_parameters(
                  entry, errors, std::format("{}.{}", current_path, arr.size())
                );
                if (res) {
                  arr.emplace_back(std::move(res.value()));
                }
              }
              if (pre_error_size != errors.size()) {
                return std::nullopt;
              }
              return arr;
            } else {
              errors.insert_type_error(current_path, "Expected a list");
              return std::nullopt;
            }
          },
          v.variant()
        );
      }

      friend bool operator==(const list &l, const list &r) {
        return *l.child == *r.child;
      }

      static list make(type_schema child) {
        return list{.type{"list"}, .child{std::make_unique<type_schema>(std::move(child))}};
      }
    };
    struct record {
      rfl::Literal<"record"> type;
      std::unique_ptr<type_schema> child;

      record clone() const {
        return record{.type{type}, .child{std::make_unique<type_schema>(child->clone())}};
      }

      std::optional<rfl::Generic::Object> validate_parameters(
        const rfl::Generic &v,
        type_schema_validation_error &errors,
        std::string_view current_path = ""
      ) const {
        return std::visit(
          [&](const auto &v) -> std::optional<rfl::Generic::Object> {
            using value_type = std::decay_t<decltype(v)>;
            if constexpr (std::same_as<value_type, rfl::Generic::Object>) {
              rfl::Generic::Object obj;
              size_t pre_error_size = errors.size();
              for (const auto &[k, v] : v) {
                auto res =
                  child->validate_parameters(v, errors, std::format("{}.{}", current_path, k));
                if (res) {
                  obj[k] = std::move(res.value());
                }
              }
              if (errors.size() != pre_error_size) {
                return std::nullopt;
              }
              return obj;
            } else {
              errors.insert_type_error(current_path, "Expected a record type");
              return std::nullopt;
            }
          },
          v.variant()
        );
      }

      friend bool operator==(const record &l, const record &r) {
        return *l.child == *r.child;
      }

      static record make(type_schema child) {
        return record{.type{"record"}, .child{std::make_unique<type_schema>(std::move(child))}};
      }
    };
    struct typed_record {
      rfl::Literal<"typed_record"> type;
      generic::key_vector<std::string, type_schema> child;

      typed_record &add(const std::string &name, const type_schema &schema) {
        child.insert(name, schema);
        return *this;
      }

      std::optional<rfl::Generic::Object> validate_parameters(
        const rfl::Generic::Object &v,
        type_schema_validation_error &errors,
        std::string_view current_path = ""
      ) const {
        rfl::Generic::Object obj;
        size_t pre_error_size = errors.size();
        for (const auto &[key, schema] : child) {
          auto entry = v.get(key).value_or(rfl::Generic{});
          auto res =
            schema.validate_parameters(entry, errors, std::format("{}.{}", current_path, key));
          if (res) {
            obj[key] = std::move(res.value());
          }
        }
        if (pre_error_size != errors.size()) {
          return std::nullopt;
        }
        return obj;
      }
      std::optional<rfl::Generic::Object> validate_parameters(
        const rfl::Generic &v,
        type_schema_validation_error &errors,
        std::string_view current_path = ""
      ) const {
        return std::visit(
          [&](const auto &v) -> std::optional<rfl::Generic::Object> {
            using value_type = std::decay_t<decltype(v)>;
            if constexpr (std::same_as<value_type, rfl::Generic::Object>) {
              return validate_parameters(v, errors, current_path);
            } else {
              errors.insert_type_error(current_path, "Not a record");
              return std::nullopt;
            }
          },
          v.variant()
        );
      }
      /*
        The two functions following this comment can be easily templated but for the sake of brevity
        with the python bindings, we retype the same function twice sadly.
      */
      std::expected<rfl::Generic::Object, type_schema_validation_error> validate_parameters(
        const rfl::Generic &v, std::string_view current_path = ""
      ) const {
        using result_type = std::expected<rfl::Generic::Object, type_schema_validation_error>;
        auto errors = core::type_schema_validation_error::make_empty();
        return validate_parameters(v, errors, current_path)
          .transform([](auto &&res) { return result_type{std::move(res)}; })
          .or_else([&]() { return std::optional{result_type{std::unexpected{std::move(errors)}}}; })
          .value();
      }
      std::expected<rfl::Generic::Object, type_schema_validation_error> validate_parameters(
        const rfl::Generic::Object &v, std::string_view current_path = ""
      ) const {
        using result_type = std::expected<rfl::Generic::Object, type_schema_validation_error>;
        auto errors = core::type_schema_validation_error::make_empty();
        return validate_parameters(v, errors, current_path)
          .transform([](auto &&res) { return result_type{std::move(res)}; })
          .or_else([&]() { return std::optional{result_type{std::unexpected{std::move(errors)}}}; })
          .value();
      }

      friend bool operator==(const typed_record &l, const typed_record &r) {
        if (l.child.size() != r.child.size()) {
          return false;
        }
        for (const auto &[l_name, l_schema] : l.child) {
          auto r_child = r.child.get(l_name);
          if (!r_child || r_child->get() != l_schema) {
            return false;
          }
        }
        return true;
      }

      static typed_record make() {
        return typed_record{.type{"typed_record"}, .child{}};
      }
    };
    struct _union {
      rfl::Literal<"union"> type;
      std::vector<type_schema> child;

      _union &add(const type_schema &schema) {
        auto it = std::ranges::find(child, schema);
        if (it == child.end()) {
          child.emplace_back(schema);
        }
        return *this;
      }

      std::optional<rfl::Generic> validate_parameters(
        const rfl::Generic &v,
        type_schema_validation_error &errors,
        std::string_view current_path = ""
      ) const {
        for (const auto &entry : child) {
          auto res = entry.validate_parameters(v);
          if (res) {
            return std::move(res.value());
          }
        }
        errors.insert_type_error(current_path, "No matching type in union");
        return std::nullopt;
      }

      friend bool operator==(const _union &l, const _union &r) {
        if (l.child.size() != r.child.size()) {
          return false;
        }
        for (const auto &l_schema : l.child) {
          auto it = std::ranges::find(r.child, l_schema);
          if (it == r.child.end()) {
            return false;
          }
        }
        return true;
      }

      static _union make() {
        return _union{.type{"union"}, .child{}};
      }
    };

    using variant_type = generic::variant<basic, list, record, typed_record, _union, literal>;
    variant_type value;

    template <class... Args>
      requires(std::is_constructible_v<variant_type, Args...>)
    type_schema(Args &&...args) : value{std::forward<Args>(args)...} {}
    type_schema() : type_schema{basic{.type{"none"}}} {}
    type_schema(const type_schema &schema) : value{std::move(schema.clone().value)} {}
    type_schema &operator=(const type_schema &schema) {
      value = std::move(schema.clone().value);
      return *this;
    }

    type_schema clone() const {
      return value.visit([](const auto &schema) {
        using schema_type = std::decay_t<decltype(schema)>;
        if constexpr (std::is_copy_constructible_v<schema_type>) {
          return type_schema{schema};
        } else {
          return type_schema{schema.clone()};
        }
      });
    }

    std::optional<rfl::Generic> validate_parameters(
      const rfl::Generic &v,
      type_schema_validation_error &errors,
      std::string_view current_path = ""
    ) const {
      return value.visit([&](const auto &schema) {
        return schema.validate_parameters(v, errors, current_path).transform([](auto &&v) {
          return rfl::Generic{std::move(v)};
        });
      });
    }

    std::expected<rfl::Generic, type_schema_validation_error> validate_parameters(
      const rfl::Generic &v
    ) const {
      using expected_type = std::expected<rfl::Generic, type_schema_validation_error>;
      auto errors = type_schema_validation_error::make_empty();
      return value.visit([&](const auto &schema) {
        auto validated_res = schema.validate_parameters(v, errors, "");
        if (validated_res) {
          return expected_type{std::move(validated_res.value())};
        } else {
          return expected_type{std::unexpected{std::move(errors)}};
        }
      });
    }

    friend bool operator==(const type_schema &l, const type_schema &r) {
      return l.value.visit([&](const auto &l) {
        return r.value.visit([&](const auto &r) {
          using l_value_type = std::decay_t<decltype(l)>;
          using r_value_type = std::decay_t<decltype(r)>;
          if constexpr (std::same_as<l_value_type, r_value_type>) {
            return l == r;
          } else {
            return false;
          }
        });
      });
    }
  };

  export using basic_type_schema = type_schema::basic;
  export using list_type_schema = type_schema::list;
  export using record_type_schema = type_schema::record;
  export using typed_record_type_schema = type_schema::typed_record;
  export using union_type_schema = type_schema::_union;
  export using literal_type_schema = type_schema::literal;
}

namespace core = pharmaco::core;

template <class T, class char_type>
  requires(
    std::same_as<T, core::basic_type_schema> || std::same_as<T, core::list_type_schema> ||
    std::same_as<T, core::record_type_schema> || std::same_as<T, core::typed_record_type_schema> ||
    std::same_as<T, core::union_type_schema> || std::same_as<T, core::type_schema> ||
    std::same_as<T, core::literal_type_schema>
  )
struct std::formatter<T, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const core::basic_type_schema &schema, auto &ctx) const {
    return std::format_to(ctx.out(), "{}", schema.type.name());
  }
  constexpr auto format(const core::list_type_schema &schema, auto &ctx) const {
    return std::format_to(ctx.out(), "List[{}]", *schema.child);
  }
  constexpr auto format(const core::record_type_schema &schema, auto &ctx) const {
    return std::format_to(ctx.out(), "Record[str, {}]", *schema.child);
  }
  constexpr auto format(const core::literal_type_schema &schema, auto &ctx) const {
    if (schema.literals.size() == 0) {
      return std::format_to(ctx.out(), "Literal[]");
    } else {
      std::format_to(ctx.out(), "Literal[");
      for (const auto &lit : schema.literals) {
        std::format_to(ctx.out(), "{}, ", lit);
      }
      return std::format_to(ctx.out(), "]");
    }
  }
  constexpr auto format(const core::typed_record_type_schema &schema, auto &ctx) const {
    if (schema.child.size() == 0) {
      return std::format_to(ctx.out(), "TypedRecord[]");
    } else {
      std::format_to(ctx.out(), "TypedRecord[\n");
      for (const auto &[key, type] : schema.child) {
        std::format_to(ctx.out(), "{}: {},\n", key, type);
      }
      return std::format_to(ctx.out(), "]");
    }
  }
  constexpr auto format(const core::union_type_schema &schema, auto &ctx) const {
    if (schema.child.size() == 0) {
      return std::format_to(ctx.out(), "Union[]");
    } else {
      std::format_to(ctx.out(), "Union[");
      for (const auto &entry : schema.child) {
        std::format_to(ctx.out(), "{}, ", entry);
      }
      return std::format_to(ctx.out(), "]");
    }
  }
  constexpr auto format(const core::type_schema &schema, auto &ctx) const {
    schema.value.visit([&](const auto &schema) { format(schema, ctx); });
    return ctx.out();
  }
};

// Type Instatiation
template rfl::Result<rfl::Generic> rfl::json::read<rfl::Generic>(
  const std::string_view, const yyjson_read_flag
);
template rfl::Result<rfl::Generic> rfl::yaml::read<rfl::Generic>(const std::string &);