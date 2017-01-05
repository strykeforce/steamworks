target_sources(sidewinder
  PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/expo.cc"
  PUBLIC
  "${CMAKE_CURRENT_LIST_DIR}/expo.h"
)

target_include_directories(sidewinder PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
