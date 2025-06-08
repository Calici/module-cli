module;
#include <pybind11/pybind11.h>
#include <filesystem>
#include <optional>
export module pharmaco.core.module_:bindings_python_embedding;
import pharmaco.core.bindings.python;
import :dir;

namespace py = pybind11;
namespace core_py = pharmaco::core::python;
namespace fs = std::filesystem;
namespace pharmaco::core {
  struct module_dir_loader {
    constexpr static const char name[] = "dir";
    void operator()(py::module_ &m) const {
      py::class_<module_dir>(m, "ModuleDir")
        .def("dir", &module_dir::dir)
        .def("run_path", &module_dir::run_path)
        .def("depends_path", &module_dir::depends_path)
        .def("logs_path", &module_dir::logs_path)
        .def("stdout_path", &module_dir::stdout_path)
        .def("stderr_path", &module_dir::stderr_path)
        .def("params_path", &module_dir::params_path)
        .def("write_module_parameters", &module_dir::write_module_parameters)
        .def("clear_content", &module_dir::clear_content)
        .def_static("make_dir", &module_dir::make_dir);
    }
  };
}