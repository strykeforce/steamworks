cmake_minimum_required (VERSION 3.1.0 FATAL_ERROR)
project(deadeye)

set(VERSION_MAJOR 17)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
set(VERSION_META "")

set(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
find_package(OpenCV  2.4 REQUIRED)
add_subdirectory(vendor)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(src)
