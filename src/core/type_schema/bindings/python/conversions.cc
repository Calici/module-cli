module;
#include <pybind11/pybind11.h>
#include <optional>
#include <string>
export module pharmaco.core.type_schema:bindings_python_conversions;
import :type_schema;

namespace py = pybind11;
namespace pharmaco::core {
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

// namespace PYBIND11_NAMESPACE {
//   namespace detail {
//     namespace core = pharmaco::core;
//     template <> struct type_caster<core::type_schema> {
//     public:
//       PYBIND11_TYPE_CASTER(core::type_schema, const_name("type_schema"));
//       bool load(handle src, bool) {
//         auto res = core::from_pybind_handle<core::type_schema>(src);
//         if (!res) {
//           return false;
//         }
//         value = std::move(res.value());
//         return true;
//       }
//     };
//     template <> struct type_caster<core::typed_record_type_schema> {
//     public:
//       PYBIND11_TYPE_CASTER(core::typed_record_type_schema, const_name("typed_record_type_schema"));
//       bool load(handle src, bool) {
//         if (!py::hasattr(src, "__annotations__")) {
//           return false;
//         } else {
//           return core::from_pybind_handle<core::typed_record_type_schema>(src.annotations())
//             .transform([&](auto &&schema) {
//               value = std::move(schema);
//               return true;
//             })
//             .value_or(false);
//         }
//       }
//     };
//   }
// }