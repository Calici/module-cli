module;
#include <pybind11/pybind11.h>
#include <expected>
#include <string_view>
export module pharmaco.manifest:plugins_python_function_module;
import :error;

namespace pharmaco::manifest {
  namespace py = pybind11;
  class function_module {
    py::module_ __module;

  public:
    function_module(py::module_ module) : __module(std::move(module)) {}

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

    template <class T, class... Args>
    std::expected<std::optional<T>, plugin_error> run_function(
      std::string_view attr, Args &&...args
    ) const {
      return get_attribute<py::function>(attr).and_then(
        [&](auto &&func) -> std::expected<std::optional<T>, plugin_error> {
          if (!func) {
            return std::nullopt;
          } else {
            try {
              return func.value()(std::forward<Args>(args)...).template cast<T>();
            } catch (const std::exception &e) {
              return std::unexpected{plugin_error{attr.data(), e.what()}};
            }
          }
        }
      );
    }
    template <class T, class... Args>
    std::expected<T, plugin_error> run_required_function(
      std::string_view attr, Args &&...args
    ) const {
      return run_function<T>(attr, std::forward<Args>(args)...).and_then([&](auto &&opt) {
        if (!opt) {
          return std::expected<T, plugin_error>{std::unexpected{plugin_error{attr, "not_found"}}};
        }
        return std::expected<T, plugin_error>{std::move(opt).value()};
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

    static std::expected<function_module, py::error_already_set> load(std::string_view module_name){
        try {
        py::module_ module = py::module_::import(module_name.data());
        return function_module{std::move(module)};
      } catch (const py::error_already_set &e) {
        return std::unexpected{e};
      }
    }
  };
}