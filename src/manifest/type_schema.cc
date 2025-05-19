module;
#include <rfl/json.hpp>
#include <rfl/yaml.hpp>
#include <format>
#include <memory>
#include <rfl.hpp>
#include <string>
#include <string_view>
export module pharmaco.manifest:type_schema;
import moderna.generic;
import :error;

namespace generic = moderna::generic;
namespace pharmaco::manifest {
  struct type_schema {
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
    };
    struct literal {
      rfl::Literal<"literal"> type;
      std::vector<std::string> literals;

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
    };
    struct list {
      rfl::Literal<"list"> type;
      std::unique_ptr<type_schema> child;

      std::optional<rfl::Generic::Array> validate_parameters(
        const rfl::Generic &v,
        type_schema_validation_error &errors,
        std::string_view current_path = ""
      ) const {
        return std::visit(
          [&](const auto &v) -> std::optional<rfl::Generic::Array> {
            using value_type = std::decay_t<decltype(v)>;
            type_schema_validation_error errors;
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
              if (pre_error_size == errors.size()) {
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
    };
    struct record {
      rfl::Literal<"record"> type;
      std::unique_ptr<type_schema> child;

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
    };
    struct typed_record {
      rfl::Literal<"typed_record"> type;
      generic::key_vector<std::string, type_schema> child;

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
              for (const auto &[key, schema] : child) {
                auto entry = v.get(key).value_or(rfl::Generic{});
                auto res = schema.validate_parameters(
                  entry, errors, std::format("{}.{}", current_path, key)
                );
                if (res) {
                  obj[key] = std::move(res.value());
                }
              }
              if (pre_error_size != errors.size()) {
                return std::nullopt;
              }
              return obj;
            } else {
              errors.insert_type_error(current_path, "Not a record");
              return std::nullopt;
            }
          },
          v.variant()
        );
      }
    };
    struct _union {
      rfl::Literal<"union"> type;
      std::vector<type_schema> child;

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
    };
    generic::variant<basic, list, record, typed_record, _union, literal> value;

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
  };

  export using basic_type_schema = type_schema::basic;
  export using list_type_schema = type_schema::list;
  export using record_type_schema = type_schema::record;
  export using typed_record_type_schema = type_schema::typed_record;
  export using union_type_schema = type_schema::_union;
  export using literal_type_schema = type_schema::literal;
}

namespace manifest = pharmaco::manifest;

template <class T, class char_type>
  requires(
    std::same_as<T, manifest::basic_type_schema> || std::same_as<T, manifest::list_type_schema> ||
    std::same_as<T, manifest::record_type_schema> ||
    std::same_as<T, manifest::typed_record_type_schema> ||
    std::same_as<T, manifest::union_type_schema> || std::same_as<T, manifest::type_schema> ||
    std::same_as<T, manifest::literal_type_schema>
  )
struct std::formatter<T, char_type> {
  constexpr auto parse(auto &ctx) {
    return ctx.begin();
  }
  constexpr auto format(const manifest::basic_type_schema &schema, auto &ctx) const {
    return std::format_to(ctx.out(), "{}", schema.type.name());
  }
  constexpr auto format(const manifest::list_type_schema &schema, auto &ctx) const {
    return std::format_to(ctx.out(), "List[{}]", *schema.child);
  }
  constexpr auto format(const manifest::record_type_schema &schema, auto &ctx) const {
    return std::format_to(ctx.out(), "Record[str, {}]", *schema.child);
  }
  constexpr auto format(const manifest::literal_type_schema &schema, auto &ctx) const {
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
  constexpr auto format(const manifest::typed_record_type_schema &schema, auto &ctx) const {
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
  constexpr auto format(const manifest::union_type_schema &schema, auto &ctx) const {
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
  constexpr auto format(const manifest::type_schema &schema, auto &ctx) const {
    schema.value.visit([&](const auto &schema) { format(schema, ctx); });
    return ctx.out();
  }
};

// Type Instatiation
template rfl::Result<rfl::Generic> rfl::json::read<rfl::Generic>(
  const std::string_view, const yyjson_read_flag
);
template rfl::Result<rfl::Generic> rfl::yaml::read<rfl::Generic>(const std::string &);