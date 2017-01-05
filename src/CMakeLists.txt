add_library(sidewinder "")

include(oi/CMakeLists.txt)
include(swerve/CMakeLists.txt)
include(talon/CMakeLists.txt)

install(TARGETS sidewinder
  ARCHIVE DESTINATION lib
)

install(DIRECTORY oi DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY swerve DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY talon DESTINATION include FILES_MATCHING PATTERN "*.h")
