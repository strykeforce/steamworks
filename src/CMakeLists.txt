configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/deadeye_config.h.in"
  "${PROJECT_BINARY_DIR}/deadeye_config.h"
  )

# add the binary tree to the search path for include files
# so that we will find deadeye_config.h
include_directories("${PROJECT_BINARY_DIR}")

file(GLOB deadeye_SRC_LIST "*.cc" )

add_executable(deadeye ${deadeye_SRC_LIST})
target_include_directories(deadeye PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(deadeye ${OpenCV_LIBS} cpptoml spdlog)
set_target_properties(deadeye PROPERTIES
  CXX_STANDARD 11
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED YES)
