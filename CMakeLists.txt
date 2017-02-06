cmake_minimum_required (VERSION 3.1.0 FATAL_ERROR)
project(deadeye)

set(VERSION_MAJOR 17)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
set(VERSION_META "")

set(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
find_package(OpenCV  2.4 REQUIRED)

# BOILER_LOOKUP_INPUT is used by vendor/tools/generate_lookup. It is set here so
# we can set it separately in test/CMakeLists.txt. It needs to be set prior to
# adding the vendor subdirectory.
set(BOILER_LOOKUP_INPUT "${CMAKE_SOURCE_DIR}/conf/steamworks_boiler_data.xlsx")
add_subdirectory(vendor)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(src)
