add_executable(avenger
  oi.cc
  robot_map.cc
  robot.cc
)

include(commands/CMakeLists.txt)
include(subsystems/CMakeLists.txt)

target_include_directories(avenger
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_SOURCE_DIR}/vendor/grapher"
)

target_link_libraries(avenger
  PUBLIC
  sidewinder
  wpi
)
