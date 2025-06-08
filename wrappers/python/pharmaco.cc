#include <pybind11/detail/common.h>
#include <pybind11/pybind11.h>
import pharmaco.core.curl;
import pharmaco.core.zip;
import pharmaco.core.type_schema;
import pharmaco.core.io_nodes;
import pharmaco.core.api;
import pharmaco.manifest;
import pharmaco.core.bindings.python;

namespace py = pybind11;
namespace core_py = pharmaco::core::python;
namespace core = pharmaco::core;
namespace manifest = pharmaco::manifest;

PYBIND11_MODULE(pharmaco, m) {
  auto core_m = m.def_submodule("core");
  core_py::load_python_module(m, manifest::python_loader{});
  core_py::load_python_module(
    core_m,
    core::zip_python_loader{},
    core::type_schema_python_loader{},
    core::io_nodes_python_loader{},
    core::api_python_loader{},
    core::curl_python_loader{}
  );
}