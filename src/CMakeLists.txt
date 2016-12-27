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
  "${PROJECT_BINARY_DIR}/src"
)

target_link_libraries(avenger
  PUBLIC
  sidewinder
  grapher
  wpi
)

add_dependencies(avenger generate_version generate_default_config)

# We delete version.h before each build. This forces a recompile of robot.cc
# every build which is acceptable in order to have a guarenteed incrementing
# build number for inspection during roboRIO deployment and execution.
set(VERSION_H "${PROJECT_BINARY_DIR}/src/version.h")

# this moves rather than deletes so linter_clang doesn't complain about missing
# version.h. We point the linter to this version.h in .clang_complete.
add_custom_command(TARGET avenger POST_BUILD
  COMMAND mv -f ${VERSION_H} "${PROJECT_BINARY_DIR}/vendor/tools/generate_version/"
)
add_custom_target(generate_version DEPENDS ${VERSION_H})

add_custom_command(
  OUTPUT ${VERSION_H}
  COMMAND "${PROJECT_BINARY_DIR}/vendor/tools/generate_version/version_gen"
  -P AVENGER
  -M ${VERSION_MAJOR}
	-m ${VERSION_MINOR}
	-p ${VERSION_PATCH}
	-a "${VERSION_META}"
	-F "${PROJECT_BINARY_DIR}/buildnumber"
	-i
	> ${VERSION_H}
  DEPENDS version_gen
  VERBATIM
)
