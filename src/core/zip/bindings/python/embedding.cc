module;
#include <pybind11/pybind11.h>
#include <filesystem>
#include <string_view>
export module pharmaco.core.zip:bindings_python_embedding;
import pharmaco.core.bindings.python;
import :error;
import :builder;

namespace py = pybind11;
namespace fs = std::filesystem;
namespace core_py = pharmaco::core::python;
namespace pharmaco::core {
  export struct zip_python_loader {
    static constexpr const char name[] = "zip";

    void operator()(py::module_ &m) {
      core_py::bind_exception<zip_error>(m, "ZipError");
      py::class_<zip_builder>(m, "ZipBuilder")
        .def("add_file", py::overload_cast<const fs::path &>(&zip_builder::add_file))
        .def(
          "add_file", py::overload_cast<std::string_view, std::string_view>(&zip_builder::add_file)
        )
        .def("add_directory", &zip_builder::add_directory)
        .def("add_to_zip", &zip_builder::add_to_zip);
    }
  };
}