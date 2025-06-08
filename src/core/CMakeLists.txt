cmake_minimum_required (VERSION 3.28)
project (pharmaco_core)
set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED true)
set (CMAKE_CXX_SCAN_FOR_MODULES true)
set (CMAKE_CXX_COMPILE_FEATURES "${CMAKE_CXX_COMPILE_FEATURES} -Wall -Werror")
option (PHARMACO_CORE_BUILD_PYTHON "Build Python Plugin into the library" ON)

# 8. libzip
if (NOT TARGET libzip_FOUND)
  find_package(libzip REQUIRED)
endif()

# 9. curl
if (NOT TARGET CURL_FOUND)
  find_package(CURL REQUIRED)
endif()

if (NOT TARGET pharmaco_core_curl)
  add_library(pharmaco_core_curl EXCLUDE_FROM_ALL)
  file (
    GLOB pharmaco_core_curl_src
    "${CMAKE_CURRENT_LIST_DIR}/curl/*.cc"
  )
  target_sources(pharmaco_core_curl PUBLIC
    FILE_SET CXX_MODULES FILES
      ${pharmaco_core_curl_src}
  )
  target_link_libraries(pharmaco_core_curl
    PRIVATE
      moderna_generic
      ${CURL_LIBRARIES}
      reflectcpp
  )
  if (PHARMACO_CORE_BUILD_PYTHON) 
    target_compile_definitions(pharmaco_core_curl PRIVATE BUILD_PYTHON_PLUGIN="")
    target_link_libraries(pharmaco_core_curl
      PRIVATE
        pharmaco_core_bindings_python
        pybind11::module
    )
    target_sources(pharmaco_core_curl PUBLIC
      FILE_SET CXX_MODULES FILES
        ${CMAKE_CURRENT_LIST_DIR}/curl/bindings/python/embedding.cc
    )
  endif()
endif()

if (NOT TARGET pharmaco_core_bindings_python AND PHARMACO_CORE_BUILD_PYTHON)
  add_library(pharmaco_core_bindings_python EXCLUDE_FROM_ALL)
  file (
    GLOB pharmaco_core_bindings_python_src
    "${CMAKE_CURRENT_LIST_DIR}/bindings/python/*.cc"
  )
  target_sources(pharmaco_core_bindings_python PUBLIC
    FILE_SET CXX_MODULES FILES
      ${pharmaco_core_bindings_python_src}
  )
  target_link_libraries(pharmaco_core_bindings_python
    PRIVATE
      pybind11::module
      moderna_generic
      reflectcpp
  )
  target_compile_definitions(pharmaco_core_bindings_python PRIVATE BUILD_PYTHON_PLUGIN="")
endif()

if (NOT TARGET pharmaco_core_api)
  add_library(pharmaco_core_api)
  file (
    GLOB pharmaco_core_api_src
    "${CMAKE_CURRENT_LIST_DIR}/api/*.cc"
    "${CMAKE_CURRENT_LIST_DIR}/api/module/*.cc"
    "${CMAKE_CURRENT_LIST_DIR}/api/developer/*.cc"
  )
  target_sources(pharmaco_core_api PUBLIC
    FILE_SET CXX_MODULES FILES
      ${pharmaco_core_api_src}
  )
  target_link_libraries(pharmaco_core_api
    PRIVATE
      pharmaco_core_curl
      moderna_process
      reflectcpp
  )
  if (PHARMACO_CORE_BUILD_PYTHON) 
    target_compile_definitions(pharmaco_core_api PRIVATE BUILD_PYTHON_PLUGIN="")
    target_link_libraries(pharmaco_core_api
      PRIVATE
        pharmaco_core_bindings_python
        pybind11::module
    )
    target_sources(pharmaco_core_api PUBLIC
      FILE_SET CXX_MODULES FILES
        ${CMAKE_CURRENT_LIST_DIR}/api/bindings/python/embedding.cc
    )
  endif()
endif()

if (NOT TARGET pharmaco_core_runner) 
  add_library(pharmaco_core_runner EXCLUDE_FROM_ALL)
  file (
    GLOB pharmaco_core_runner_src
    "${CMAKE_CURRENT_LIST_DIR}/runner/*.cc"
  )
  target_sources(pharmaco_core_runner PUBLIC
    FILE_SET CXX_MODULES FILES
      ${pharmaco_core_runner_src}
  )
  target_link_libraries(pharmaco_core_runner
    PRIVATE
      reflectcpp
      moderna_io 
      pharmaco_core_module
      moderna_generic
      moderna_process
  )
  if (PHARMACO_CORE_BUILD_PYTHON) 
    target_compile_definitions(pharmaco_core_runner PRIVATE BUILD_PYTHON_PLUGIN="")
    target_link_libraries(pharmaco_core_runner
      PRIVATE
        pharmaco_core_bindings_python
        pybind11::module
    )
    target_sources(pharmaco_core_runner PUBLIC
      FILE_SET CXX_MODULES FILES
        ${CMAKE_CURRENT_LIST_DIR}/runner/bindings/python/embedding.cc
    )
  endif()
endif()

if (NOT TARGET pharmaco_core_zip) 
  add_library(pharmaco_core_zip EXCLUDE_FROM_ALL)
  file (
    GLOB pharmaco_core_zip_src
    "${CMAKE_CURRENT_LIST_DIR}/zip/*.cc"
  )
  target_sources(pharmaco_core_zip PUBLIC
    FILE_SET CXX_MODULES FILES
      ${pharmaco_core_zip_src}
  )
  target_link_libraries(pharmaco_core_zip
    PRIVATE
      reflectcpp
      moderna_io 
      moderna_generic
      moderna_process
      libzip::zip
  )
  if (PHARMACO_CORE_BUILD_PYTHON) 
    target_compile_definitions(pharmaco_core_zip PRIVATE BUILD_PYTHON_PLUGIN="")
    target_link_libraries(pharmaco_core_zip
      PRIVATE
        pharmaco_core_bindings_python
        pybind11::module
    )
    target_sources(pharmaco_core_zip PUBLIC
      FILE_SET CXX_MODULES FILES
        ${CMAKE_CURRENT_LIST_DIR}/zip/bindings/python/embedding.cc
    )
  endif()
endif()

if (NOT TARGET pharmaco_core_module) 
  add_library(pharmaco_core_module EXCLUDE_FROM_ALL)
  file (
    GLOB pharmaco_core_module_src
    "${CMAKE_CURRENT_LIST_DIR}/module_/*.cc"
  )
  target_sources(pharmaco_core_module PUBLIC
    FILE_SET CXX_MODULES FILES
      ${pharmaco_core_module_src}
  )
  target_link_libraries(pharmaco_core_module
    PRIVATE
      reflectcpp
      moderna_io 
      moderna_generic
      moderna_process
  )
  if (PHARMACO_CORE_BUILD_PYTHON) 
    target_compile_definitions(pharmaco_core_module PRIVATE BUILD_PYTHON_PLUGIN="")
    target_link_libraries(pharmaco_core_module
      PRIVATE
        pharmaco_core_bindings_python
        pybind11::module
    )
    target_sources(pharmaco_core_module PUBLIC
      FILE_SET CXX_MODULES FILES
        ${CMAKE_CURRENT_LIST_DIR}/module_/bindings/python/embedding.cc
    )
  endif()
endif()

if (NOT TARGET pharmaco_core_type_schema) 
  add_library(pharmaco_core_type_schema EXCLUDE_FROM_ALL)
  file (
    GLOB pharmaco_core_type_schema_src
    "${CMAKE_CURRENT_LIST_DIR}/type_schema/*.cc"
  )
  target_sources(pharmaco_core_type_schema PUBLIC
    FILE_SET CXX_MODULES FILES
      ${pharmaco_core_type_schema_src}
  )
  target_link_libraries(pharmaco_core_type_schema
    PRIVATE
      reflectcpp
      moderna_generic
  )
  if (PHARMACO_CORE_BUILD_PYTHON) 
    target_compile_definitions(pharmaco_core_type_schema PRIVATE BUILD_PYTHON_PLUGIN="")
    target_link_libraries(pharmaco_core_type_schema
      PRIVATE
        pharmaco_core_bindings_python
        pybind11::module
    )
    target_sources(pharmaco_core_type_schema PUBLIC
      FILE_SET CXX_MODULES FILES
        ${CMAKE_CURRENT_LIST_DIR}/type_schema/bindings/python/embedding.cc
        ${CMAKE_CURRENT_LIST_DIR}/type_schema/bindings/python/conversions.cc
    )
  endif()
endif()

if (NOT TARGET pharmaco_core_io_nodes) 
  add_library(pharmaco_core_io_nodes EXCLUDE_FROM_ALL)
  file (
    GLOB pharmaco_core_io_nodes_src
    "${CMAKE_CURRENT_LIST_DIR}/io_nodes/*.cc"
  )
  target_sources(pharmaco_core_io_nodes PUBLIC
    FILE_SET CXX_MODULES FILES
      ${pharmaco_core_io_nodes_src}
  )
  target_link_libraries(pharmaco_core_io_nodes
    PRIVATE
      reflectcpp
      moderna_generic
      moderna_type_check
  )
  if (PHARMACO_CORE_BUILD_PYTHON) 
    target_compile_definitions(pharmaco_core_io_nodes PRIVATE BUILD_PYTHON_PLUGIN="")
    target_link_libraries(pharmaco_core_io_nodes
      PRIVATE
        pharmaco_core_bindings_python
        pybind11::module
    )
    target_sources(pharmaco_core_io_nodes PUBLIC
      FILE_SET CXX_MODULES FILES
        ${CMAKE_CURRENT_LIST_DIR}/io_nodes/bindings/python/embedding.cc
    )
  endif()
endif()

if (TARGET moderna_test_lib) 
  moderna_add_test(pharmaco_runner_test
    ${CMAKE_CURRENT_LIST_DIR}/tests/runner.cc
    LIBRARIES pharmaco_core_runner
    SANITIZERS all
  )
endif()