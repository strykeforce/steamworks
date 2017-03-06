target_sources(${CMAKE_PROJECT_NAME}
  PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/deadeye_azimuth.cc"
  "${CMAKE_CURRENT_LIST_DIR}/gyro_azimuth.cc"
  "${CMAKE_CURRENT_LIST_DIR}/drive.cc"
  "${CMAKE_CURRENT_LIST_DIR}/position_azimuth.cc"
  "${CMAKE_CURRENT_LIST_DIR}/position_drive.cc"
)
