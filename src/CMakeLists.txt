
include_directories(
"${CMAKE_SOURCE_DIR}/vendor/include"
"${CMAKE_SOURCE_DIR}/vendor/serial/include"
"${PROJECT_BINARY_DIR}/src"
/opt/flycapture/include
)

link_directories(/opt/flycapture/lib)

add_executable(${CMAKE_PROJECT_NAME}
  main.cc
  boiler_camera.cc
  boiler_frame.cc
  gear_camera.cc
  gear_frame.cc
  link.cc
  deadeye.cc
)

include(link/CMakeLists.txt)

target_link_libraries(${CMAKE_PROJECT_NAME}
  pthread
  ${OpenCV_LIBS}
  flycapture
  serial
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
  CXX_STANDARD 11
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED YES)
