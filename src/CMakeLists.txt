add_library(sidewinder "")

include(swerve/CMakeLists.txt)
include(talon/CMakeLists.txt)

install(TARGETS sidewinder
  ARCHIVE DESTINATION lib
)

install(DIRECTORY swerve DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY talon DESTINATION include FILES_MATCHING PATTERN "*.h")
