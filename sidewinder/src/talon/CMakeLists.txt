target_sources(sidewinder
  PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/settings.cc"
  "${CMAKE_CURRENT_LIST_DIR}/voltage_settings.cc"
  "${CMAKE_CURRENT_LIST_DIR}/pid_settings.cc"
  "${CMAKE_CURRENT_LIST_DIR}/position_settings.cc"
  "${CMAKE_CURRENT_LIST_DIR}/velocity_settings.cc"
  "${CMAKE_CURRENT_LIST_DIR}/motion_magic_settings.cc"
  PUBLIC
  "${CMAKE_CURRENT_LIST_DIR}/settings.h"
)

target_include_directories(sidewinder PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
