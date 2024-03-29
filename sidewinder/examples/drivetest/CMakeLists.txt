set(DRIVETEST "${CMAKE_PROJECT_NAME}-drivetest")
add_executable(${DRIVETEST}
  oi.cc
  robot_map.cc
  robot.cc
)

include(commands/CMakeLists.txt)
include(subsystems/CMakeLists.txt)

target_include_directories(${DRIVETEST}
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${DRIVETEST}
  PUBLIC
  sidewinder
  CTRLib
  CTRLibDriver
  navx_frc_cpp
  wpi
)

set(
DEPLOY_DEST
${CMAKE_PROJECT_NAME}-${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}${VERSION_META})

add_custom_target(deploy
COMMAND /bin/bash ${CMAKE_BINARY_DIR}/vendor/tools/deploy/deploy.sh
VERBATIM
DEPENDS ${DRIVETEST}
COMMENT "Deploying ${DEPLOY_DEST} to roboRIO"
)
