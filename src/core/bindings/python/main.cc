module;
#include <pybind11/pybind11.h>
#include <rfl.hpp>
export module pharmaco.core.bindings.python;
export import :function_module;
import :expected_caster;
import :rfl_generic_caster;
import :generic_variant_caster;
import moderna.generic;

namespace py = pybind11;
namespace pharmaco::core::python {
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
      return load_python_module(m, std::forward<loaders>(modules)...);
    }
  }

  export template <class enum_type> void bind_enum(py::module_ &m, std::string_view name) {
    auto e = py::enum_<enum_type>(m, name.data());
    rfl::get_enumerators<enum_type>().apply([&](const auto &...fields) {
      (e.value(fields.name().data(), fields.value()), ...);
    });
  }

  export template <class exception_type, typename... Args>
    requires(std::is_base_of_v<std::exception, exception_type>)
  py::class_<exception_type, Args...> bind_exception(
    py::module_ &m, std::string_view name, Args &&...args
  ) {
    return py::class_<exception_type, Args...>(m, name.data(), std::forward<Args>(args)...)
      .def("__str__", &exception_type::what)
      .def("__repr__", &exception_type::what)
      .def("what", &exception_type::what);
  }

  export template <class T> struct decl {};

  export template <class T, std::invocable<const T &> F = std::identity> struct string_maker {
    F proj;
    string_maker(F &&f, decl<T> v) : proj{std::forward<F>(f)} {}
    string_maker(decl<T> v)
      requires(std::is_default_constructible_v<F>)
    {}

    std::string operator()(const T &v) const {
      return std::format("{}", std::invoke(proj, v));
    }
  };

  export struct default_string_maker {
    std::string operator()(const std::formattable<char> auto &v) const {
      return std::format("{}", v);
    }
  };

  export template <class T, std::invocable<const T &> IterBeg, std::invocable<const T &> IterEnd>
  struct iterator_maker {
    IterBeg beg;
    IterEnd end;
    iterator_maker(IterBeg &&beg, IterEnd &&end, decl<T> v) :
      beg{std::forward<IterBeg>(beg)}, end{std::forward<IterEnd>(end)} {}

    auto operator()(const T &v) const {
      return py::make_iterator(std::invoke(beg, v), std::invoke(end, v));
    }
  };
}