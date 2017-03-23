target_sources(${CMAKE_PROJECT_NAME}
  PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/deploy.cc"
  "${CMAKE_CURRENT_LIST_DIR}/load.cc"
  "${CMAKE_CURRENT_LIST_DIR}/pivot.cc"
  "${CMAKE_CURRENT_LIST_DIR}/sequence.cc"
  "${CMAKE_CURRENT_LIST_DIR}/clamp.cc"
  "${CMAKE_CURRENT_LIST_DIR}/release.cc"
)

include("${CMAKE_CURRENT_LIST_DIR}/auton/CMakeLists.txt")
