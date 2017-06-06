target_sources(sidewinder
  PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/swerve_drive.cc"
  "${CMAKE_CURRENT_LIST_DIR}/swerve_math.cc"
  "${CMAKE_CURRENT_LIST_DIR}/swerve_math.h"
  PUBLIC
  "${CMAKE_CURRENT_LIST_DIR}/swerve_drive.h"
  "${CMAKE_CURRENT_LIST_DIR}/talon_map.h"
)

target_include_directories(sidewinder PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
