target_sources(${CMAKE_PROJECT_NAME}
  PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/azimuth.cc"
  "${CMAKE_CURRENT_LIST_DIR}/log.cc"
  "${CMAKE_CURRENT_LIST_DIR}/hopper.cc"
  "${CMAKE_CURRENT_LIST_DIR}/intake.cc"
)

include("${CMAKE_CURRENT_LIST_DIR}/drive/CMakeLists.txt")
