cmake_minimum_required (VERSION 2.8.0 FATAL_ERROR)
project(deadeye CXX)
set (deadeye_VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

find_package(OpenCV  2.4 REQUIRED)

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/vendor/include")

add_subdirectory(src)
