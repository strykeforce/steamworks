set(GRAPHER ON CACHE BOOL "Include grapher support")

configure_file("robot_config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/robot_config.h")

add_executable(drivetest
  robot_map.cc
  robot.cc
)

target_include_directories(drivetest
PRIVATE
${CMAKE_CURRENT_BINARY_DIR}
"${CMAKE_SOURCE_DIR}/vendor/grapher"
)

target_link_libraries(drivetest PUBLIC
  sidewinder
  grapher
  wpi
)
