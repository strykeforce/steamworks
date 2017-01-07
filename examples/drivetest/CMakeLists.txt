add_executable(drivetest
  oi.cc
  robot_map.cc
  robot.cc
)

include(commands/CMakeLists.txt)
include(subsystems/CMakeLists.txt)

target_include_directories(drivetest
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_SOURCE_DIR}/vendor/grapher"
)

target_link_libraries(drivetest
  PUBLIC
  sidewinder
  grapher
  TalonSRXLib
  wpi
)
