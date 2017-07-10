cmake_minimum_required(VERSION 3.2)
project(deadeye-test)

set(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
find_package(OpenCV  2.4 REQUIRED)

add_subdirectory(
  ${CMAKE_CURRENT_SOURCE_DIR}/../vendor
  ${CMAKE_CURRENT_BINARY_DIR}/vendor
)
add_definitions(-DDEADEYE_TEST)

add_executable(test-runner
  test-runner.cc
  deadeye_test.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/deadeye.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/link.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/camera.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/frame.cc
)

set(INCLUDE_DIRS
${CMAKE_CURRENT_SOURCE_DIR}/../vendor/include
${CMAKE_CURRENT_SOURCE_DIR}/../src
"${PROJECT_BINARY_DIR}/src"
)

find_file(HAVE_FLYCAPTURE_OPT FlyCapture2.h
PATHS /opt/flycapture/include/flycapture NO_DEFAULT_PATH)

if(HAVE_FLYCAPTURE_OPT)
  list(APPEND INCLUDE_DIRS /opt/flycapture/include)
endif(HAVE_FLYCAPTURE_OPT)

target_include_directories(test-runner PUBLIC ${INCLUDE_DIRS})

target_link_libraries(test-runner ${OpenCV_LIBS} flycapture)

set_target_properties(test-runner
  PROPERTIES
  CXX_STANDARD 11
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED YES
)
