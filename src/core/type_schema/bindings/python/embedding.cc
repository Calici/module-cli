module;
#include <pybind11/pybind11.h>
#include <rfl.hpp>
export module pharmaco.core.type_schema:bindings_python_embedding;
import pharmaco.core.bindings.python;
import :error;
import :bindings_python_conversions;
import :type_schema;

namespace py = pybind11;
namespace core_py = pharmaco::core::python;
namespace pharmaco::core {
  export struct type_schema_python_loader {
    static constexpr const char name[] = "type_schema";
    void operator()(py::module_ &m) {
      py::class_<type_schema_error>(m, "TypeSchemaError")
        .def_readonly("error", &type_schema_error::error)
        .def_static("value_error", &type_schema_error::value_error)
        .def_static("type_error", &type_schema_error::type_error)
        .def("__repr__", core_py::string_maker{core_py::decl<type_schema_error>{}})
        .def("__str__", core_py::string_maker{core_py::decl<type_schema_error>{}});
      py::class_<type_schema_validation_error>(m, "TypeSchemaValidationError")
        .def(py::init<>())
        .def("insert_type_error", &type_schema_validation_error::insert_type_error)
        .def("insert_value_error", &type_schema_validation_error::insert_value_error)
        .def("empty", &type_schema_validation_error::empty)
        .def(
          "__iter__",
          core_py::iterator_maker{
            &type_schema_validation_error::begin,
            &type_schema_validation_error::end,
            core_py::decl<type_schema_validation_error>{}
          },
          py::keep_alive<0, 1>()
        )
        .def("__repr__", core_py::string_maker{core_py::decl<type_schema_validation_error>{}})
        .def("__str__", core_py::string_maker{core_py::decl<type_schema_validation_error>{}})
        .def("__len__", &type_schema_validation_error::size)
        .def_static("make_empty", &type_schema_validation_error::make_empty);
      py::class_<type_schema>(m, "TypeSchema")
        .def("__str__", core_py::string_maker{core_py::decl<type_schema>{}})
        .def("__repr__", core_py::string_maker{core_py::decl<type_schema>{}})
        .def(py::init([](py::object obj) {
          auto schema = core::from_pybind_handle<core::type_schema>(obj);
          if (!schema) {
            throw py::cast_error{};
          }
          return std::move(schema.value());
        }))
        .def(
          "validate_parameters",
          py::overload_cast<const rfl::Generic &>(&type_schema::validate_parameters, py::const_)
        )
        .def(
          "validate_parameters",
          py::overload_cast<const rfl::Generic &, type_schema_validation_error &, std::string_view>(
            &type_schema::validate_parameters, py::const_
          )
        )
        .def_static("from_typedef", core::from_pybind_handle<type_schema>);
      py::class_<typed_record_type_schema>(m, "TypedRecordTypeSchema")
        .def("__str__", core_py::string_maker{core_py::decl<typed_record_type_schema>{}})
        .def("__repr__", core_py::string_maker{core_py::decl<typed_record_type_schema>{}})
        .def(py::init([](py::object obj) {
          auto schema = core::from_pybind_handle<core::typed_record_type_schema>(obj.annotations());
          if (!schema) {
            throw py::cast_error{};
          }
          return std::move(schema.value());
        }))
        .def(
          "validate_parameters",
          py::overload_cast<
            const rfl::Generic::Object &,
            type_schema_validation_error &,
            std::string_view>(&typed_record_type_schema::validate_parameters, py::const_)
        )
        .def(
          "validate_parameters",
          py::overload_cast<const rfl::Generic &, type_schema_validation_error &, std::string_view>(
            &typed_record_type_schema::validate_parameters, py::const_
          )
        )
        .def(
          "validate_parameters",
          py::overload_cast<const rfl::Generic::Object &, std::string_view>(
            &typed_record_type_schema::validate_parameters, py::const_
          )
        )
        .def(
          "validate_parameters",
          py::overload_cast<const rfl::Generic &, std::string_view>(
            &typed_record_type_schema::validate_parameters, py::const_
          )
        );
    }
  };
}