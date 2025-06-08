module;
#include <pybind11/pybind11.h>
export module pharmaco.core.api:bindings_python_embedding;
import pharmaco.core.bindings.python;
import :token;
import :notification;
import :api_module;
import :generic_context;

namespace py = pybind11;
namespace core_py = pharmaco::core::python;
namespace pharmaco::core::api {
  export struct api_context_loader {
    constexpr static const char name[] = "context";
    void operator()(py::module_ &m) const {
      core_py::bind_exception<context_error>(m, "ContextError").def("attr", &context_error::attr);
      py::class_<id_extra_context>(m, "IdExtraContext").def_readonly("id", &id_extra_context::id);
      py::class_<module_context>(m, "ModuleContext")
        .def_readonly("base_url", &module_context::base_url)
        .def_readonly("auth_token", &module_context::auth_token)
        .def_readonly("extras", &module_context::extras)
        .def_static("get_global_context", &module_context::get_global_context);
    }
  };
  export struct api_notification_loader {
    constexpr static const char name[] = "notification";
    void operator()(py::module_ &m) const {
      m.def("create_notification", &notification::create);
      core_py::bind_enum<notification_type>(m, "NotificationType");
    }
  };
  export struct api_token_loader {
    constexpr static const char name[] = "token";
    void operator()(py::module_ &m) const {
      m.def("use_token", api::token::use);
      m.def("set_token", api::token::set);
      m.def("use_all_token", api::token::use_all);
    }
  };
  export struct api_module_loader {
    constexpr static const char name[] = "module";
    void operator()(py::module_ &m) const {
      py::class_<api_module_data<false>>(m, "ModuleData")
        .def_readonly("progress", &api_module_data<false>::progress)
        .def_readonly("parameters", &api_module_data<false>::parameters)
        .def_readonly("results", &api_module_data<false>::results)
        .def_readonly("status", &api_module_data<false>::status)
        .def_readonly("major_update_type", &api_module_data<false>::major_update_type)
        .def_readonly("id", &api_module_data<false>::id);
      py::class_<api_module>(m, "APIModule")
        .def("data", &api_module::data)
        .def("set_progress", &api_module::set_progress)
        .def("set_results", &api_module::set_results)
        .def("set_major_update_type", &api_module::set_major_update_type)
        .def("set_status", &api_module::set_status)
        .def("push", &api_module::push)
        .def_static("make", &api_module::make);
      core_py::bind_enum<module_status>(m, "ModuleStatus");
      core_py::bind_enum<module_update_type>(m, "ModuleUpdateType");
    }
  };
  export struct python_loader {
    constexpr static const char name[] = "api";
    void operator()(py::module_ &m) const {
      core_py::load_python_module(
        m, api_context_loader{}, api_notification_loader{}, api_token_loader{}, api_module_loader{}
      );
    }
  };
}

namespace pharmaco::core {
  export using api_python_loader = api::python_loader;
}