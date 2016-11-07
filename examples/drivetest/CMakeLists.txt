add_executable(drivetest
  robot_map.cc
  robot.cc
)

target_link_libraries(drivetest PUBLIC
  sidewinder
  wpi
)
