
include_directories(
"${CMAKE_SOURCE_DIR}/vendor/include"
/opt/flycapture/include
)

link_directories(/opt/flycapture/lib)

add_executable(deadeye
main.cc
camera.cc
frame.cc
deadeye.cc
)

target_link_libraries(deadeye ${OpenCV_LIBS} flycapture)
set_target_properties(deadeye PROPERTIES
  CXX_STANDARD 11
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED YES)
