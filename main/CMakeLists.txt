set(deadeye_linked_libs deadeye_core ${OpenCV_LIBS} cpptoml spdlog)
# add the binary tree to the search path for include files
# so that we will find deadeye_config.h
include_directories("${PROJECT_BINARY_DIR}")

add_executable(deadeyed deadeyed.cc)
target_link_libraries(deadeyed ${deadeye_linked_libs})
set_target_properties(deadeyed PROPERTIES
  CXX_STANDARD 11
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED YES)

add_executable(pipeline pipeline.cc)
target_link_libraries(pipeline ${deadeye_linked_libs})
set_target_properties(pipeline PROPERTIES
  CXX_STANDARD 11
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED YES)

add_executable(deadeye-snap snap.cc)
target_link_libraries(deadeye-snap ${deadeye_linked_libs})
set_target_properties(deadeye-snap PROPERTIES
  CXX_STANDARD 11
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED YES)

install(TARGETS deadeyed pipeline deadeye-snap
  DESTINATION bin
  CONFIGURATIONS Release)
