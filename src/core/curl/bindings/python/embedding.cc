module;
#include <curl/curl.h>
#include <pybind11/pybind11.h>
export module pharmaco.core.curl:bindings_python_embedding;
import pharmaco.core.bindings.python;
import :error;

namespace py = pybind11;
namespace core_py = pharmaco::core::python;
namespace pharmaco::core {
  export struct curl_python_loader {
    static constexpr const char name[] = "curl";
    void operator()(py::module_ &m) {
      core_py::bind_exception<curl_error>(m, "CurlError").def("code", &curl_error::code);
    }
  };
}