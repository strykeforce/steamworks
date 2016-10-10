cmake_minimum_required (VERSION 3.1.0 FATAL_ERROR)
project(deadeye CXX)
set (deadeye_VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV  2.4 REQUIRED)
message(STATUS "OpenCV_VERSION: " ${OpenCV_VERSION} )

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/vendor/include")

add_subdirectory(src)
