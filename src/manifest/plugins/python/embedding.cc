module;
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <rfl/Generic.hpp>
#include <filesystem>
#include <string_view>
export module pharmaco.manifest:plugins_python_embedding;
import :type_schema;
import :io_nodes;
import :plugins_python_conversions;
import :zip_builder;

namespace py = pybind11;
namespace ph = pharmaco::manifest;

template <class T> struct decl {};

template <class T, std::invocable<const T &> IterBeg, std::invocable<const T &> IterEnd>
struct iterator_maker {
  IterBeg beg;
  IterEnd end;
  iterator_maker(IterBeg &&beg, IterEnd &&end, decl<T> v) :
    beg{std::forward<IterBeg>(beg)}, end{std::forward<IterEnd>(end)} {}

  auto operator()(const T &v) const {
    return py::make_iterator(std::invoke(beg, v), std::invoke(end, v));
  }
};

template <class T, std::invocable<const T &> F = std::identity> struct string_maker {
  F proj;
  string_maker(F &&f, decl<T> v) : proj{std::forward<F>(f)} {}
  string_maker(decl<T> v)
    requires(std::is_default_constructible_v<F>)
  {}

  std::string operator()(const T &v) const {
    return std::format("{}", std::invoke(proj, v));
  }
};

namespace pharmaco::manifest {
  export struct python_module_loader {
    constexpr static const char name[] = "manifest";
    void operator()(py::module_ &m) const {
      py::class_<ph::type_schema_error>(m, "TypeSchemaError")
        .def_readonly("error", &ph::type_schema_error::error)
        .def_static("value_error", &ph::type_schema_error::value_error)
        .def_static("type_error", &ph::type_schema_error::type_error)
        .def("__repr__", string_maker{decl<ph::type_schema_error>{}})
        .def("__str__", string_maker{decl<ph::type_schema_error>{}});
      py::class_<ph::type_schema_validation_error>(m, "TypeSchemaValidationError")
        .def(py::init<>())
        .def("insert_type_error", &ph::type_schema_validation_error::insert_type_error)
        .def("insert_value_error", &ph::type_schema_validation_error::insert_value_error)
        .def("empty", &ph::type_schema_validation_error::empty)
        .def(
          "__iter__",
          iterator_maker{
            &ph::type_schema_validation_error::begin,
            &ph::type_schema_validation_error::end,
            decl<ph::type_schema_validation_error>{}
          },
          py::keep_alive<0, 1>()
        )
        .def("__repr__", string_maker{decl<ph::type_schema_validation_error>{}})
        .def("__str__", string_maker{decl<ph::type_schema_validation_error>{}})
        .def("__len__", &ph::type_schema_validation_error::size)
        .def_static("make_empty", &ph::type_schema_validation_error::make_empty);
      py::class_<ph::io_nodes>(m, "IONodes")
        .def(py::init<>())
        .def("insert", py::overload_cast<std::string_view, std::string_view>(&ph::io_nodes::insert))
        .def("__len__", &ph::io_nodes::size)
        .def(
          "__iter__", iterator_maker{&ph::io_nodes::begin, &ph::io_nodes::end, decl<ph::io_nodes>{}}
        )
        .def("__repr__", string_maker{decl<ph::io_nodes>{}})
        .def("__str__", string_maker{decl<ph::io_nodes>{}})
        .def_static("make_empty", &ph::io_nodes::make_empty);
      py::class_<ph::zip_builder>(m, "ZipBuilder")
        .def("add_file", py::overload_cast<const fs::path &>(&ph::zip_builder::add_file))
        .def(
          "add_file",
          py::overload_cast<std::string_view, std::string_view>(&ph::zip_builder::add_file)
        )
        .def("add_directory", &ph::zip_builder::add_directory)
        .def("add_to_zip", &ph::zip_builder::add_to_zip);
    }
  };
}