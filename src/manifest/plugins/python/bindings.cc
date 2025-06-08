module;
#include <pybind11/pybind11.h>
export module pharmaco.manifest:plugins_python_bindings;
import :loaded_plugin;
import :plugins_python_py_plugin;
import :plugin_io;
import :error;

namespace pharmaco::manifest {
  namespace py = pybind11;
  export struct python_loader {
    static constexpr const char name[] = "manifest";
    template <is_plugin plugin_type>
    std::pair<py::class_<loaded_plugin<plugin_type>>, py::class_<plugin_type>> bind_loaded_plugin(
      py::module_ &m, std::string_view loaded_plugin_name, std::string_view plugin_name
    ) const {
      using plugin = loaded_plugin<plugin_type>;
      return std::make_pair(
        py::class_<plugin>(m, loaded_plugin_name.data())
          .def("io", &plugin::io, py::return_value_policy::reference_internal)
          .def("plugin", &plugin::plugin, py::return_value_policy::reference_internal)
          .def("plugin_validate_in_nodes", &plugin::plugin_validate_in_nodes)
          .def("validate_in_nodes", &plugin::validate_in_nodes)
          .def("plugin_get_default_parameters", &plugin::plugin_get_default_parameters)
          .def("get_default_parameters", &plugin::get_default_parameters)
          .def("plugin_validate_parameters", &plugin::plugin_validate_parameters)
          .def("validate_parameters", &plugin::validate_parameters)
          .def("get_required_token", &plugin::get_required_token)
          .def("get_download_zip", &plugin::get_download_zip),
        py::class_<plugin_type>(m, plugin_name.data())
          .def("get_default_parameters", &plugin_type::get_default_parameters)
          .def("get_params_schema", &plugin_type::get_params_schema)
          .def("get_results_schema", &plugin_type::get_results_schema)
          .def("get_in_nodes_schema", &plugin_type::get_in_nodes_schema)
          .def("get_in_nodes", &plugin_type::get_in_nodes)
          .def("get_out_nodes", &plugin_type::get_out_nodes)
          .def("validate_in_nodes", &plugin_type::validate_in_nodes)
          .def("validate_parameters", &plugin_type::validate_parameters)
          .def("get_required_token", &plugin_type::get_required_token)
          .def("get_download_zip", &plugin_type::get_download_zip)
      );
    }
    template <is_plugin plugin_type> void bind_loader(py::module_ &m) const {
      m.def("make_plugin", make_plugin<plugin_type>);
    }
    void operator()(py::module_ &m) const {
      core_py::bind_exception<plugin_error>(m, "PluginError")
        .def("attr", &plugin_error::attr)
        .def("reason", &plugin_error::reason);
      py::class_<plugin_io>(m, "PluginIO")
        .def_readonly("params_schema", &plugin_io::params_schema)
        .def_readonly("results_schema", &plugin_io::results_schema)
        .def_readonly("in_nodes_schema", &plugin_io::in_nodes_schema)
        .def_readonly("in_nodes", &plugin_io::in_nodes)
        .def_readonly("out_nodes", &plugin_io::out_nodes)
        .def("schema_validate_parameters", &plugin_io::schema_validate_parameters)
        .def("schema_validate_results", &plugin_io::schema_validate_results)
        .def("schema_validate_in_nodes", &plugin_io::schema_validate_in_nodes)
        .def_static("make", plugin_io::make<py_plugin>);
      auto added_py_plugin = bind_loaded_plugin<py_plugin>(m, "LoadedPyPlugin", "PyPlugin");
      added_py_plugin.second.def_static("load", &py_plugin::load);
      bind_loader<py_plugin>(m);
    }
  };
}