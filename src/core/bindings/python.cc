module;
#include <pybind11/pybind11.h>
#include <concepts>
#include <string>
export module pharmaco.core:bindings_python;

namespace py = pybind11;
namespace pharmaco::core {

  export template <class module_type>
  concept is_loadable_module = requires(module_type module) {
    std::invocable<module_type, py::module_ &>;
    std::is_convertible_v<decltype(module.name), const char *>;
  };

  export template <std::invocable<py::module_ &> loader, std::invocable<py::module_ &>... loaders>
  void load_python_module(py::module_ &m, loader &&module, loaders &&...modules) {
    auto sub_m = m.def_submodule(module.name);
    std::invoke(module, sub_m);
    if constexpr (sizeof...(loaders) != 0) {
      return load_python_module(m, std::move(modules)...);
    }
  }
}