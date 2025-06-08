module;
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <rfl.hpp>
export module pharmaco.core.bindings.python:rfl_generic_caster;

namespace py = pybind11;
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
  }
}