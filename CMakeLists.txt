cmake_minimum_required(VERSION 3.2)
project(BDC_Comm CXX)

set(WPI_LIBS /opt/wpilib CACHE PATH "WPILib directory.")

set(CMAKE_CXX_COMPILER arm-frc-linux-gnueabi-g++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings -std=c++1y")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

include_directories(
  SYSTEM
  "${WPI_LIBS}/include"
)

add_subdirectory(src)
add_subdirectory(lib)
