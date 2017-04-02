cmake_minimum_required(VERSION 3.2)
project(steamworks)

set(WPI_LIBS /opt/wpilib CACHE PATH "WPILib directory.")
set(NAVX_LIBS /opt/navx CACHE PATH "NavX directory.")
set(CTRE_LIBS /opt/ctre CACHE PATH "CTRE directory.")
set(SIDEWINDER_LIBS /opt/sidewinder CACHE PATH "Sidewinder directory.")
set(MSGPACK_LIBS /opt/msgpack-c CACHE PATH "Msgpack-c directory.")

set(VERSION_MAJOR 17)
set(VERSION_MINOR 2)
set(VERSION_PATCH 10)
set(VERSION_META "")

set(CMAKE_CXX_COMPILER /usr/bin/arm-frc-linux-gnueabi-g++)
set(CMAKE_LD           /usr/bin/arm-frc-linux-gnueabi-ld)
set(CMAKE_AR           /usr/bin/arm-frc-linux-gnueabi-ar)
set(CMAKE_RANLIB       /usr/bin/arm-frc-linux-gnueabi-ranlib)
set(CMAKE_OBJCOPY      /usr/bin/arm-frc-linux-gnueabi-objcopy)
set(CMAKE_OBJCOPY      /usr/bin/arm-frc-linux-gnueabi-objdump)
set(CMAKE_NM           /usr/bin/arm-frc-linux-gnueabi-nm)
set(CMAKE_STRIP        /usr/bin/arm-frc-linux-gnueabi-strip)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -fstrict-aliasing -std=c++1y")

# spdlog options
add_definitions(-DSPDLOG_NO_THREAD_ID)
if(CMAKE_BUILD_TYPE MATCHES "Debug")
  add_definitions(-DSPDLOG_DEBUG_ON -DSPDLOG_TRACE_ON)
endif(CMAKE_BUILD_TYPE MATCHES "Debug")

include_directories(
  SYSTEM
  "${SIDEWINDER_LIBS}/include"
  "${WPI_LIBS}/include"
  "${NAVX_LIBS}/include"
  "${CTRE_LIBS}/include"
  "${CMAKE_SOURCE_DIR}/vendor/include"
  "${MSGPACK_LIBS}/include"
)

link_directories(
  "${SIDEWINDER_LIBS}/lib"
  "${WPI_LIBS}/lib"
  "${NAVX_LIBS}/lib"
  "${CTRE_LIBS}/lib"
)

add_subdirectory(src)

set(DEFAULT_CONFIG_H "${PROJECT_BINARY_DIR}/src/default_config.h")
add_subdirectory(vendor)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)

if(DOXYGEN_FOUND)
  configure_file(${PROJECT_SOURCE_DIR}/conf/Doxyfile.in
    ${PROJECT_BINARY_DIR}/Doxyfile @ONLY)

  add_custom_target(doc
  ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
endif(DOXYGEN_FOUND)
