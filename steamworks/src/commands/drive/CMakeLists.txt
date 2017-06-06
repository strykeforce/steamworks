target_sources(${CMAKE_PROJECT_NAME}
  PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/tele_op.cc"
  "${CMAKE_CURRENT_LIST_DIR}/drive_zero.cc"
  "${CMAKE_CURRENT_LIST_DIR}/set_zero.cc"
  "${CMAKE_CURRENT_LIST_DIR}/zero_gyro.cc"
  "${CMAKE_CURRENT_LIST_DIR}/brake.cc"
  "${CMAKE_CURRENT_LIST_DIR}/health_check.cc"
)

include("${CMAKE_CURRENT_LIST_DIR}/auton/CMakeLists.txt")
