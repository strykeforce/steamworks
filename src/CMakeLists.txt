configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/deadeye_config.h.in"
  "${PROJECT_BINARY_DIR}/deadeye_config.h"
  )

# add the binary tree to the search path for include files
# so that we will find deadeye_config.h
include_directories("${PROJECT_BINARY_DIR}")

file(GLOB DEADEYE_SRC_LIST "*.cpp" )

add_executable (deadeye ${DEADEYE_SRC_LIST})
target_link_libraries(deadeye ${OpenCV_LIBS})
