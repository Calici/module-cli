module;
#include <pybind11/pybind11.h>
export module pharmaco.core.runner:bindings_python_embedding;
import pharmaco.core.bindings.python;
import :resources;

namespace pharmaco::core {
  namespace py = pybind11;
  namespace core_py = pharmaco::core::python;
  struct resource_python_loader {
    static constexpr char name[] = "resource";
    void operator()(py::module_ &m) const {
      py::class_<resource_guard<size_t, gpu_resource_device::deleter>>(m, "GpuResourceGuard")
        .def("data", &resource_guard<size_t, gpu_resource_device::deleter>::data)
        .def("has_value", &resource_guard<size_t, gpu_resource_device::deleter>::has_value)
        .def("free", &resource_guard<size_t, gpu_resource_device::deleter>::free);
      py::class_<gpu_resource_device_data>(m, "GpuResourceDeviceData")
        .def(py::init<std::string, size_t>())
        .def_readonly("gpu_id", &gpu_resource_device_data::gpu_id)
        .def_readonly("max_mem", &gpu_resource_device_data::max_mem);
      py::class_<gpu_resource_device>(m, "GpuResourceDevice")
        .def(py::init<std::string, size_t>())
        .def(py::init<gpu_resource_device_data>())
        .def("gpu_id", &gpu_resource_device::gpu_id)
        .def("get_max_memory", &gpu_resource_device::get_max_memory)
        .def("get_empty_memory", &gpu_resource_device::get_empty_memory)
        .def("get_used_memory", &gpu_resource_device::get_used_memory)
        .def("try_allocate", &gpu_resource_device::try_allocate)
        .def("try_allocate_max", &gpu_resource_device::try_allocate_max)
        .def("alloc_amount_or_max", &gpu_resource_device::alloc_amount_or_max);
      py::class_<gpu_resource_device_list>(m, "GpuResourceDeviceList")
        .def(py::init<std::vector<gpu_resource_device_data>>())
        .def("try_allocate", &gpu_resource_device_list::try_allocate)
        .def(
          "get_device", py::overload_cast<size_t>(&gpu_resource_device_list::get_device, py::const_)
        )
        .def(
          "get_device",
          py::overload_cast<std::string_view>(&gpu_resource_device_list::get_device, py::const_)
        )
        .def(
          "get_max_memory",
          py::overload_cast<std::string_view>(&gpu_resource_device_list::get_max_memory, py::const_)
        )
        .def(
          "get_max_memory",
          py::overload_cast<size_t>(&gpu_resource_device_list::get_max_memory, py::const_)
        )
        .def(
          "get_max_memory",
          py::overload_cast<>(&gpu_resource_device_list::get_max_memory, py::const_)
        )
        .def(
          "get_used_memory",
          py::overload_cast<std::string_view>(
            &gpu_resource_device_list::get_used_memory, py::const_
          )
        )
        .def(
          "get_used_memory",
          py::overload_cast<size_t>(&gpu_resource_device_list::get_used_memory, py::const_)
        )
        .def(
          "get_used_memory",
          py::overload_cast<>(&gpu_resource_device_list::get_used_memory, py::const_)
        )
        .def(
          "get_empty_memory",
          py::overload_cast<std::string_view>(
            &gpu_resource_device_list::get_empty_memory, py::const_
          )
        )
        .def(
          "get_empty_memory",
          py::overload_cast<size_t>(&gpu_resource_device_list::get_empty_memory, py::const_)
        )
        .def(
          "get_empty_memory",
          py::overload_cast<>(&gpu_resource_device_list::get_empty_memory, py::const_)
        )
        .def("device_count", &gpu_resource_device_list::device_count);
    }
  };
  struct process_python_loader {
    static constexpr char name[] = "process";
    void operator()(py::module_ &m) const {}
  };

  struct runner_python_loader {
    static constexpr char name[] = "runner";
    void operator()(py::module_ &m) const {
      core_py::load_python_module(m, resource_python_loader{}, process_python_loader{});
    }
  };
}