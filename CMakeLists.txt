cmake_minimum_required (VERSION 3.1.0 FATAL_ERROR)
project(deadeye CXX)
set (deadeye_VERSION 0.1.0)

find_package(OpenCV  2.4 REQUIRED)

add_library(cpptoml INTERFACE)
target_include_directories(cpptoml INTERFACE
  ${PROJECT_SOURCE_DIR}/vendor/include)

add_library(spdlog INTERFACE)
target_include_directories(spdlog INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/vendor/include>)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(src)
