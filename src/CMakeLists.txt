
include_directories(
"${CMAKE_SOURCE_DIR}/vendor/include"
"${PROJECT_BINARY_DIR}/src"
/opt/flycapture/include
/opt/msgpack-c/include
)

link_directories(/opt/flycapture/lib)

add_executable(${CMAKE_PROJECT_NAME}
  main.cc
  link.cc
  deadeye.cc
  fps.cc
)

include("${CMAKE_CURRENT_LIST_DIR}/boiler/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/gear/CMakeLists.txt")

target_link_libraries(${CMAKE_PROJECT_NAME}
  pthread
  ${OpenCV_LIBS}
  flycapture
)

install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION sbin)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
  CXX_STANDARD 11
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED YES)
