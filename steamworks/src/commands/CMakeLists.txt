target_sources(${CMAKE_PROJECT_NAME}
  PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/log.cc"
  "${CMAKE_CURRENT_LIST_DIR}/hopper.cc"
  "${CMAKE_CURRENT_LIST_DIR}/intake.cc"
  "${CMAKE_CURRENT_LIST_DIR}/dashboard.cc"
)

include("${CMAKE_CURRENT_LIST_DIR}/auton/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/climber/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/deadeye/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/drive/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/gear/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/shooter/CMakeLists.txt")
