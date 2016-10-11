# add the binary tree to the search path for include files
# so that we will find deadeye_config.h
include_directories("${PROJECT_BINARY_DIR}")

add_executable(deadeye deadeye.cc)
target_link_libraries(deadeye deadeye_core ${OpenCV_LIBS} cpptoml spdlog)
set_target_properties(deadeye PROPERTIES
  CXX_STANDARD 11
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED YES)
