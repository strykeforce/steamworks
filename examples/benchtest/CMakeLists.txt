add_executable(benchtest
  robot_map.cc
  robot.cc
)

target_link_libraries(benchtest PUBLIC
  sidewinder
  "wpi"
)
