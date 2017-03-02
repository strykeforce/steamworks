target_sources(${CMAKE_PROJECT_NAME}
  PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/tele_op.cc"
  "${CMAKE_CURRENT_LIST_DIR}/zero.cc"
)

include("${CMAKE_CURRENT_LIST_DIR}/auton/CMakeLists.txt")
