target_sources(sidewinder
  PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/swerve_drive.cc"
  PUBLIC
  "${CMAKE_CURRENT_LIST_DIR}/talon_map.h"
  "${CMAKE_CURRENT_LIST_DIR}/swerve_drive.h"
)

target_include_directories(sidewinder PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
