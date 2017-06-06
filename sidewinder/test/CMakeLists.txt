cmake_minimum_required(VERSION 3.2)
project(sidewinder-test)

add_executable(test-runner
  test-runner.cc
  swerve.cc
  ../src/swerve/swerve_math.cc
)

target_include_directories(test-runner
  PUBLIC
  ../vendor/include
  ../src
)

set_target_properties(test-runner
  PROPERTIES
  CXX_STANDARD 11
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED YES
)
