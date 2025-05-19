module;
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <expected>
#include <rfl.hpp>
export module pharmaco.manifest:plugins_python_conversions;
import moderna.generic;
import :type_schema;

namespace py = pybind11;
namespace generic = moderna::generic;

namespace pharmaco::manifest {
  template <class T> std::optional<T> from_pybind_handle(py::handle) = delete;
  template <class T> py::object to_pybind_handle(const T &) = delete;

  template <> std::optional<list_type_schema> from_pybind_handle(py::handle);
  template <> std::optional<record_type_schema> from_pybind_handle(py::handle);
  template <> std::optional<typed_record_type_schema> from_pybind_handle(py::handle);
  template <> std::optional<literal_type_schema> from_pybind_handle(py::handle);
  template <> std::optional<type_schema> from_pybind_handle(py::handle);

  template <> std::optional<list_type_schema> from_pybind_handle(py::handle h) {
    try {
      auto args = h.attr("__args__").cast<py::list>();
      auto converted_args = from_pybind_handle<type_schema>(args[0]);
      return converted_args.transform([](auto &&schema) {
        return list_type_schema{
          .type{"list"}, .child{std::make_unique<type_schema>(std::move(schema))}
        };
      });
    } catch (const py::cast_error &e) {
      return std::nullopt;
    } catch (const py::attribute_error &e) {
      return std::nullopt;
    }
  }

  template <> std::optional<record_type_schema> from_pybind_handle(py::handle h) {
    try {
      auto args = h.attr("__args__").cast<py::list>();
      if (args[0].attr("__name__").cast<std::string>() != "str") {
        return std::nullopt;
      }
      auto converted_args = from_pybind_handle<type_schema>(args[1]);
      return converted_args.transform([](auto &&schema) {
        return record_type_schema{
          .type{"record"}, .child{std::make_unique<type_schema>(std::move(schema))}
        };
      });
    } catch (const py::cast_error &e) {
      return std::nullopt;
    } catch (const py::attribute_error &e) {
      return std::nullopt;
    }
  }

  template <> std::optional<typed_record_type_schema> from_pybind_handle(py::handle h) {
    try {
      auto type_map = h.cast<py::dict>();
      typed_record_type_schema schema{.type{"typed_record"}, .child{}};
      for (const auto &[raw_key, raw_type] : type_map) {
        auto key = raw_key.cast<std::string>();
        auto type = from_pybind_handle<type_schema>(raw_type);
        if (!type) {
          return std::nullopt;
        }
        schema.child.insert(key, std::move(type.value()));
      }
      return schema;
    } catch (const py::cast_error &e) {
      return std::nullopt;
    }
  }
  template <> std::optional<union_type_schema> from_pybind_handle(py::handle h) {
    try {
      union_type_schema schema;
      auto args = h.attr("__args__").cast<py::list>();
      for (const auto &arg : args) {
        auto arg_schema = from_pybind_handle<type_schema>(arg);
        if (!arg_schema) {
          return std::nullopt;
        }
        schema.child.emplace_back(std::move(arg_schema.value()));
      }
      return schema;
    } catch (const py::cast_error &e) {
      return std::nullopt;
    } catch (const py::attribute_error &e) {
      return std::nullopt;
    }
  }
  template <> std::optional<literal_type_schema> from_pybind_handle(py::handle h) {
    try {
      literal_type_schema schema;
      auto args = h.attr("__args__").cast<py::list>();
      for (const auto &arg : args) {
        schema.literals.emplace_back(arg.template cast<std::string>());
      }
      return schema;
    } catch (const py::cast_error &e) {
      return std::nullopt;
    } catch (const py::attribute_error &e) {
      return std::nullopt;
    }
  }
  template <> std::optional<type_schema> from_pybind_handle(py::handle h) {
    auto to_schema = [](auto &&schema) { return type_schema{std::move(schema)}; };
    if (py::hasattr(h, "__annotations__")) {
      return from_pybind_handle<typed_record_type_schema>(h.annotations()).transform(to_schema);
    } else if (py::hasattr(h, "__name__")) {
      auto name = h.attr("__name__").cast<std::string>();
      if (name == "NoneType") return type_schema{basic_type_schema{.type{"none"}}};
      else if (name == "int")
        return type_schema{basic_type_schema{.type{"int"}}};
      else if (name == "float")
        return type_schema{basic_type_schema{.type{"float"}}};
      else if (name == "bool")
        return type_schema{basic_type_schema{.type{"bool"}}};
      else if (name == "str")
        return type_schema{basic_type_schema{.type{"str"}}};
      else if (name == "List" || name == "list")
        return from_pybind_handle<list_type_schema>(h).transform(to_schema);
      else if (name == "Dict" || name == "dict")
        return from_pybind_handle<record_type_schema>(h).transform(to_schema);
      else if (name == "Union" || name == "Optional")
        return from_pybind_handle<union_type_schema>(h).transform(to_schema);
      else if (name == "Literal")
        return from_pybind_handle<literal_type_schema>(h).transform(to_schema);
      else {
        return std::nullopt;
      }
    } else if (py::hasattr(h, "__args__")) {
      return from_pybind_handle<union_type_schema>(h).transform(to_schema);
    } else {
      return std::nullopt;
    }
  }
}
namespace rfl {
  template <class T> std::optional<T> from_pybind_handle(py::handle) = delete;
  template <class T> py::object to_pybind_handle(const T &) = delete;

  template <> std::optional<Generic> from_pybind_handle(py::handle);
  template <> std::optional<Generic::Object> from_pybind_handle(py::handle);
  template <> std::optional<Generic::Array> from_pybind_handle(py::handle);
  template <> py::object to_pybind_handle<Generic>(const Generic &);
  template <> py::object to_pybind_handle<Generic::Array>(const Generic::Array &);
  template <> py::object to_pybind_handle<Generic::Object>(const Generic::Object &);

  /*
    Implement
  */
  template <> std::optional<Generic::Object> from_pybind_handle(py::handle h) {
    try {
      auto raw_obj = h.cast<py::dict>();
      Generic::Object converted_obj;
      for (const auto &[raw_key, raw_value] : raw_obj) {
        auto converted_key = raw_key.cast<std::string>();
        auto converted_value = from_pybind_handle<rfl::Generic>(raw_value);
        if (!converted_value) {
          return std::nullopt;
        }
        converted_obj[std::move(converted_key)] = std::move(converted_value.value());
      }
      return converted_obj;
    } catch (const py::cast_error &e) {
      return std::nullopt;
    }
  }

  template <> std::optional<Generic::Array> from_pybind_handle(py::handle h) {
    try {
      auto raw_arr = h.cast<py::list>();
      Generic::Array converted_arr;
      for (const auto &raw_value : raw_arr) {
        auto converted_value = from_pybind_handle<Generic>(raw_value);
        if (!converted_value) {
          return std::nullopt;
        }
        converted_arr.emplace_back(std::move(converted_value.value()));
      }
      return converted_arr;
    } catch (const py::cast_error &e) {
      return std::nullopt;
    }
  }

  template <> std::optional<Generic> from_pybind_handle(py::handle h) {
    if (h.is_none()) {
      return rfl::Generic{std::nullopt};
    } else if (py::isinstance<py::bool_>(h)) {
      return rfl::Generic{h.cast<bool>()};
    } else if (py::isinstance<py::int_>(h)) {
      return rfl::Generic{h.cast<int>()};
    } else if (py::isinstance<py::float_>(h)) {
      return rfl::Generic{h.cast<double>()};
    } else if (py::isinstance<py::str>(h)) {
      return rfl::Generic{h.cast<std::string>()};
    } else if (py::isinstance<py::list>(h)) {
      return from_pybind_handle<Generic::Array>(h).transform([](auto &&arr) {
        return rfl::Generic{std::move(arr)};
      });
    } else if (py::isinstance<py::dict>(h)) {
      return from_pybind_handle<Generic::Object>(h).transform([](auto &&obj) {
        return rfl::Generic{std::move(obj)};
      });
    }
    return std::nullopt;
  }

  template <> py::object to_pybind_handle<Generic>(const Generic &v) {
    return std::visit(
      [](const auto &raw_value) -> py::object {
        using raw_value_type = std::decay_t<decltype(raw_value)>;
        if constexpr (std::same_as<raw_value_type, std::nullopt_t>) {
          return py::none();
        } else if constexpr (std::same_as<raw_value_type, bool>) {
          return py::bool_{raw_value};
        } else if constexpr (std::same_as<raw_value_type, int64_t>) {
          return py::int_{raw_value};
        } else if constexpr (std::same_as<raw_value_type, double>) {
          return py::float_{raw_value};
        } else if constexpr (std::same_as<raw_value_type, std::string>) {
          return py::str{raw_value.c_str()};
        } else if constexpr (std::same_as<raw_value_type, Generic::Array>) {
          return to_pybind_handle(raw_value);
        } else if constexpr (std::same_as<raw_value_type, Generic::Object>) {
          return to_pybind_handle(raw_value);
        }
      },
      v.variant()
    );
  }
  template <> py::object to_pybind_handle<Generic::Array>(const Generic::Array &raw_arr) {
    py::list converted_arr;
    for (const auto &v : raw_arr) {
      converted_arr.append(to_pybind_handle(v));
    }
    return converted_arr;
  }
  template <> py::object to_pybind_handle<Generic::Object>(const Generic::Object &raw_obj) {
    py::dict converted_obj{};
    for (const auto &[k, v] : raw_obj) {
      converted_obj[k.c_str()] = to_pybind_handle(v);
    }
    return converted_obj;
  }
}
namespace PYBIND11_NAMESPACE {
  namespace detail {
    namespace manifest = pharmaco::manifest;
    template <> struct type_caster<rfl::Generic> {
      PYBIND11_TYPE_CASTER(rfl::Generic, const_name("Generic"));
      bool load(handle src, bool) {
        return rfl::from_pybind_handle<rfl::Generic>(src)
          .transform([&](auto &&converted_value) {
            value = std::move(converted_value);
            return true;
          })
          .value_or(false);
      }
      static handle cast(const rfl::Generic &o, return_value_policy policy, handle parent) {
        return rfl::to_pybind_handle(o).release();
      }
    };
    template <> struct type_caster<rfl::Generic::Object> {
      PYBIND11_TYPE_CASTER(rfl::Generic::Object, const_name("GenericObject"));
      bool load(handle src, bool) {
        return rfl::from_pybind_handle<rfl::Generic::Object>(src)
          .transform([&](auto &&converted_value) {
            value = std::move(converted_value);
            return true;
          })
          .value_or(false);
      }
      static handle cast(const rfl::Generic::Object &o, return_value_policy policy, handle parent) {
        return rfl::to_pybind_handle(o).release();
      }
    };
    template <> struct type_caster<rfl::Generic::Array> {
      PYBIND11_TYPE_CASTER(rfl::Generic::Array, const_name("GenericArray"));
      bool load(handle src, bool) {
        return rfl::from_pybind_handle<rfl::Generic::Array>(src)
          .transform([&](auto &&converted_value) {
            value = std::move(converted_value);
            return true;
          })
          .value_or(false);
      }
      static handle cast(const rfl::Generic::Array &o, return_value_policy policy, handle parent) {
        return rfl::to_pybind_handle(o).release();
      }
    };

    template <class... Args>
    struct type_caster<generic::variant<Args...>> : public type_caster<std::variant<Args...>> {
      PYBIND11_TYPE_CASTER(generic::variant<Args...>, const_name("GenericVariant"));
      using super = type_caster<std::variant<Args...>>;
      bool load(handle src, bool p) {
        bool is_loaded = super::load(src, p);
        if (is_loaded) {
          value.value = std::move(super::value);
        }
        return is_loaded;
      }
      static handle cast(
        const generic::variant<Args...> &v, return_value_policy policy, handle parent
      ) {
        return super::cast(v.value, policy, parent);
      }
    };
    template <class T, generic::is_formattable_error E>
    struct type_caster<std::expected<T, E>> : public type_caster<T> {
    public:
      using expected_type = std::expected<T, E>;
      using super = type_caster<T>;
      PYBIND11_TYPE_CASTER(expected_type, const_name("expected"));
      static handle cast(
        const std::expected<T, E> &src, return_value_policy policy, handle parent
      ) {
        if (src) {
          return super::cast(src.value(), policy, parent);
        } else {
          throw std::runtime_error{src.error().what()};
        }
      }
    };
    template <> struct type_caster<manifest::type_schema> {
    public:
      PYBIND11_TYPE_CASTER(manifest::type_schema, const_name("type_schema"));
      bool load(handle src, bool) {
        auto res = manifest::from_pybind_handle<manifest::type_schema>(src);
        if (!res) {
          return false;
        }
        value = std::move(res.value());
        return true;
      }
    };
    template <> struct type_caster<manifest::typed_record_type_schema> {
    public:
      PYBIND11_TYPE_CASTER(
        manifest::typed_record_type_schema, const_name("typed_record_type_schema")
      );
      bool load(handle src, bool) {
        if (!py::hasattr(src, "__annotations__")) {
          return false;
        } else {
          return manifest::from_pybind_handle<manifest::typed_record_type_schema>(src.annotations())
            .transform([&](auto &&schema) {
              value = std::move(schema);
              return true;
            })
            .value_or(false);
        }
      }
    };
  }
}