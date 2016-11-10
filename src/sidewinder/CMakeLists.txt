target_sources(sidewinder
  PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/oi.cc"
  "${CMAKE_CURRENT_LIST_DIR}/swerve_drive.cc"
  "${CMAKE_CURRENT_LIST_DIR}/swerve_math.cc"
  "${CMAKE_CURRENT_LIST_DIR}/swerve_math.h"
  "${CMAKE_CURRENT_LIST_DIR}/tele_drive.cc"
  PUBLIC
  "${CMAKE_CURRENT_LIST_DIR}/oi.h"
  "${CMAKE_CURRENT_LIST_DIR}/swerve_drive.h"
  "${CMAKE_CURRENT_LIST_DIR}/talon_map.h"
  "${CMAKE_CURRENT_LIST_DIR}/tele_drive.h"
)

target_include_directories(sidewinder PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
