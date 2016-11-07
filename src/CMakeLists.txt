add_library(sidewinder "")

include(sidewinder/CMakeLists.txt)
include(talon/CMakeLists.txt)

install(TARGETS sidewinder
  ARCHIVE DESTINATION lib
)

install(DIRECTORY sidewinder DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY talon DESTINATION include FILES_MATCHING PATTERN "*.h")
