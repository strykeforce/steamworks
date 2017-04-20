cmake_minimum_required (VERSION 3.1.0 FATAL_ERROR)
project(deadeye)

set(VERSION_MAJOR 17)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)
set(VERSION_META "")

option (LOG_FPS  "Log camera FPS" OFF)
option (LOG_BOILER  "Log boiler solution data" OFF)
option (LOG_GEAR  "Log gear solution data" OFF)

set(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
find_package(OpenCV  2.4 REQUIRED)

# spdlog options
add_definitions(-DSPDLOG_NO_THREAD_ID)
if(CMAKE_BUILD_TYPE MATCHES "Debug")
  add_definitions(-DSPDLOG_DEBUG_ON -DSPDLOG_TRACE_ON)
endif(CMAKE_BUILD_TYPE MATCHES "Debug")

configure_file (
  "${PROJECT_SOURCE_DIR}/conf/config.h.in"
  "${PROJECT_BINARY_DIR}/src/config.h"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(src)
