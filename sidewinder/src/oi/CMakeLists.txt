target_sources(sidewinder
  PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/expo.cc"
  "${CMAKE_CURRENT_LIST_DIR}/sma.cc"
  PUBLIC
  "${CMAKE_CURRENT_LIST_DIR}/expo.h"
  "${CMAKE_CURRENT_LIST_DIR}/sma.h"
)

target_include_directories(sidewinder PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
