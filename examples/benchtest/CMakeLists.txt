add_executable(benchtest
  robot_map.cc
  robot.cc
)

target_include_directories(benchtest PUBLIC
  "../../src"
)

link_directories("../../src/talon")

target_link_libraries(benchtest
  talon
  "wpi"
)
