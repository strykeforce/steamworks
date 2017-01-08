add_executable(${CMAKE_PROJECT_NAME}
  oi.cc
  robot_map.cc
  robot.cc
)

include(commands/CMakeLists.txt)
include(subsystems/CMakeLists.txt)

target_include_directories(${CMAKE_PROJECT_NAME}
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${PROJECT_BINARY_DIR}/src"
)

target_link_libraries(${CMAKE_PROJECT_NAME}
  PUBLIC
  sidewinder
  TalonSRXLib
  wpi
)

add_dependencies(${CMAKE_PROJECT_NAME} generate_version generate_default_config)

# We delete version.h before each build. This forces a recompile of robot.cc
# every build which is acceptable in order to have a guarenteed incrementing
# build number for inspection during roboRIO deployment and execution.
set(VERSION_H "${PROJECT_BINARY_DIR}/src/version.h")

# this moves rather than deletes so linter_clang doesn't complain about missing
# version.h. We point the linter to this version.h in .clang_complete.
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
  COMMAND mv -f ${VERSION_H} "${PROJECT_BINARY_DIR}/vendor/tools/generate_version/"
)
add_custom_target(generate_version DEPENDS ${VERSION_H})

add_custom_command(
  OUTPUT ${VERSION_H}
  COMMAND "${PROJECT_BINARY_DIR}/vendor/tools/generate_version/version_gen"
  -P STEAMWORKS
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

set(
  DEPLOY_DEST
${CMAKE_PROJECT_NAME}.${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_META})

add_custom_target(deploy
COMMAND /bin/bash ${CMAKE_BINARY_DIR}/vendor/tools/deploy/deploy.sh
VERBATIM
DEPENDS ${CMAKE_PROJECT_NAME}
COMMENT "Deploying ${DEPLOY_DEST} to roboRIO"
)
