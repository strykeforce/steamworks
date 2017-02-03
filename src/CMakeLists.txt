
include_directories(
"${CMAKE_SOURCE_DIR}/vendor/include"
"${CMAKE_SOURCE_DIR}/vendor/serial/include"
/opt/flycapture/include
)

link_directories(/opt/flycapture/lib)

add_executable(deadeye
main.cc
camera.cc
frame.cc
link.cc
deadeye.cc
link/boiler.cc
link/checksum.cc
link/parser.cc
link/sentence.cc
)

target_link_libraries(deadeye ${OpenCV_LIBS} flycapture serial)
set_target_properties(deadeye PROPERTIES
  CXX_STANDARD 11
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED YES)
