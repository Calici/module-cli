module;
#include <pybind11/pybind11.h>
export module pharmaco.core.io_nodes:bindings_python_embedding;
import pharmaco.core.bindings.python;
import :io_nodes;

namespace py = pybind11;
namespace core_py = pharmaco::core::python;
namespace pharmaco::core {
  export struct io_nodes_python_loader {
    constexpr static const char name[] = "io_nodes";
    void operator()(py::module_ &m) const {
      py::class_<io_nodes>(m, "IONodes")
        .def(py::init<>())
        .def("insert", py::overload_cast<std::string_view, std::string_view>(&io_nodes::insert))
        .def("__len__", &io_nodes::size)
        .def(
          "__iter__",
          core_py::iterator_maker{&io_nodes::begin, &io_nodes::end, core_py::decl<io_nodes>{}}
        )
        .def("__repr__", core_py::string_maker{core_py::decl<io_nodes>{}})
        .def("__str__", core_py::string_maker{core_py::decl<io_nodes>{}})
        .def_static("make_empty", &io_nodes::make_empty);
    }
  };
}