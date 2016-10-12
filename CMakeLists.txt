cmake_minimum_required (VERSION 3.1.0 FATAL_ERROR)
project(Deadeye CXX)
set (deadeye_VERSION 0.1.0)

configure_file (
  "${PROJECT_SOURCE_DIR}/config/deadeye_config.h.in"
  "${PROJECT_BINARY_DIR}/deadeye_config.h")

find_package(OpenCV  2.4 REQUIRED)
add_subdirectory(vendor)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(src)
add_subdirectory(lib)
