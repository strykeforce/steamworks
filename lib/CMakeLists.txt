file(GLOB deadeye_core_SRC_LIST "*.cc" )
add_library(deadeye_core ${deadeye_core_SRC_LIST})
target_include_directories(deadeye_core PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(deadeye_core ${OpenCV_LIBS} cpptoml spdlog)
set_target_properties(deadeye_core PROPERTIES
  CXX_STANDARD 11
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED YES)
