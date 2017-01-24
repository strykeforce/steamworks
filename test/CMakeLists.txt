cmake_minimum_required(VERSION 3.2)
project(deadeye-test)

set(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
find_package(OpenCV  2.4 REQUIRED)

add_executable(test-runner
  test-runner.cc
  frame_test.cc
  ../src/frame.cc
)

target_include_directories(test-runner
  PUBLIC
  ../vendor/include
  ../src
)

target_link_libraries(test-runner ${OpenCV_LIBS})

set_target_properties(test-runner
  PROPERTIES
  CXX_STANDARD 11
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED YES
)

add_executable(display
  display.cc
  ../src/frame.cc
)

target_include_directories(display
  PUBLIC
  ../vendor/include
  ../src
)

target_link_libraries(display ${OpenCV_LIBS})

set_target_properties(display
  PROPERTIES
  CXX_STANDARD 11
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED YES
)
