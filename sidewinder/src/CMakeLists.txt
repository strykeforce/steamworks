add_library(sidewinder version.cc)

include(oi/CMakeLists.txt)
include(swerve/CMakeLists.txt)
include(talon/CMakeLists.txt)
include(grapher/CMakeLists.txt)

install(TARGETS sidewinder
  ARCHIVE DESTINATION lib
)

add_dependencies(sidewinder generate_version)

set(VERSION_H "${PROJECT_BINARY_DIR}/src/version_defs.h")

add_custom_target(generate_version DEPENDS ${VERSION_H})

add_custom_command(
  OUTPUT ${VERSION_H}
  COMMAND "${PROJECT_BINARY_DIR}/vendor/tools/generate_version/version_gen"
  -P SIDEWINDER
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

install(FILES version.h DESTINATION include/sidewinder)
install(DIRECTORY oi DESTINATION include/sidewinder FILES_MATCHING PATTERN "*.h")
install(DIRECTORY swerve DESTINATION include/sidewinder FILES_MATCHING PATTERN "*.h")
install(DIRECTORY talon DESTINATION include/sidewinder FILES_MATCHING PATTERN "*.h")
install(DIRECTORY grapher DESTINATION include/sidewinder FILES_MATCHING PATTERN "*.h")
