module;
#include <pybind11/pybind11.h>
#include <expected>
#include <filesystem>
#include <string_view>
#include <type_traits>
export module pharmaco.core.bindings.python:function_module;

namespace pharmaco::core::python {
  namespace py = pybind11;
  namespace fs = std::filesystem;

  export template <class plugin_error>
    requires(std::is_constructible_v<plugin_error, std::string_view, std::string_view>)
  class function_module {
    py::module_ __module;
    fs::path __path;

  public:
    function_module(py::module_ module, fs::path path) :
      __module(std::move(module)), __path{std::move(path)} {}

    const fs::path &path() const noexcept {
      return __path;
    }
    template <class T>
    std::expected<std::optional<T>, plugin_error> get_attribute(std::string_view attr) const {
      try {
        if (py::hasattr(__module, attr.data())) {
          return __module.attr(attr.data()).cast<T>();
        } else {
          return std::nullopt;
        }
      } catch (const std::exception &e) {
        return std::unexpected{plugin_error{attr, e.what()}};
      }
    }

    template <class... Args>
    std::expected<std::optional<py::object>, plugin_error> run_generic_function(
      std::string_view attr, Args &&...args
    ) const {
      return get_attribute<py::function>(attr).and_then(
        [&](auto &&func) -> std::expected<std::optional<py::object>, plugin_error> {
          if (!func) {
            return std::nullopt;
          } else {
            try {
              return func.value()(std::forward<Args>(args)...);
            } catch (const std::exception &e) {
              return std::unexpected{plugin_error{attr.data(), e.what()}};
            }
          }
        }
      );
    }
    template <class T, class... Args>
    std::expected<std::optional<T>, plugin_error> run_function(
      std::string_view attr, Args &&...args
    ) const {
      using result_type = std::expected<std::optional<T>, plugin_error>;
      return run_generic_function(attr, std::forward<Args>(args)...)
        .and_then([&](std::optional<py::object> &&obj) -> result_type {
          try {
            return std::move(obj).transform([](py::object &&obj) { return obj.cast<T>(); });
          } catch (const std::exception &e) {
            return std::unexpected{plugin_error{attr.data(), e.what()}};
          }
        });
    }
    template <class... Args>
    std::expected<py::object, plugin_error> run_required_generic_function(
      std::string_view attr, Args &&...args
    ) const {
      return run_generic_function(attr, std::forward<Args>(args)...).and_then([&](auto &&opt) {
        if (!opt) {
          return std::expected<py::object, plugin_error>{
            std::unexpected{plugin_error{attr, "not_found"}}
          };
        }
        return std::expected<py::object, plugin_error>{std::move(opt).value()};
      });
    }
    template <class T, class... Args>
    std::expected<T, plugin_error> run_required_function(
      std::string_view attr, Args &&...args
    ) const {
      using result_type = std::expected<T, plugin_error>;
      return run_required_generic_function(attr, std::forward<Args>(args)...)
        .and_then([&](py::object &&obj) -> result_type {
          try {
            return obj.cast<T>();
          } catch (const std::exception &e) {
            return std::unexpected{plugin_error{attr.data(), e.what()}};
          }
        });
    }
    template <class T>
    std::expected<T, plugin_error> get_required_attribute(std::string_view attr) const {
      return get_attribute<T>(attr).and_then([&](auto &&opt) {
        if (!opt) {
          return std::expected<T, plugin_error>{std::unexpected{plugin_error{attr, "not_found"}}};
        }
        return std::expected<T, plugin_error>{std::move(opt).value()};
      });
    }

    std::expected<void, py::error_already_set> reload() const {
      try {
        auto importlib_util = py::module_::import("importlib.util");
        importlib_util.attr("reload")(__module);
        return {};
      } catch (const py::error_already_set &e) {
        return std::unexpected{e};
      }
    }

    static std::expected<function_module, plugin_error> load(const fs::path &module_path) {
      try {
        // Some python shenanigans.
        // Read https://www.geeksforgeeks.org/how-to-import-a-python-module-given-the-full-path/
        // import file using importlib
        // This allows us to import anything anywhere without worrying about namespace collision
        auto importlib_util = py::module_::import("importlib.util");
        auto spec = importlib_util.attr("spec_from_file_location")(
          module_path.stem().c_str(), module_path.c_str()
        );
        py::module_ module = importlib_util.attr("module_from_spec")(spec);
        spec.attr("loader").attr("exec_module")(module);
        return function_module{std::move(module), module_path};
      } catch (const py::error_already_set &e) {
        return std::unexpected{plugin_error{"", e.what()}};
      }
    }
  };
}