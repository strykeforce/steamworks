target_sources(sidewinder
  PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/talon.cc"
  "${CMAKE_CURRENT_LIST_DIR}/voltage_talon.cc"
  "${CMAKE_CURRENT_LIST_DIR}/pid_talon.cc"
  "${CMAKE_CURRENT_LIST_DIR}/position_talon.cc"
  PUBLIC
  "${CMAKE_CURRENT_LIST_DIR}/talon.h"
  "${CMAKE_CURRENT_LIST_DIR}/voltage_talon.h"
  "${CMAKE_CURRENT_LIST_DIR}/pid_talon.h"
  "${CMAKE_CURRENT_LIST_DIR}/position_talon.h"
)

target_include_directories(sidewinder PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
