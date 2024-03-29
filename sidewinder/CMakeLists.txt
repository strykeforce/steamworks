cmake_minimum_required(VERSION 3.2)
project(sidewinder)

set(WPI_LIBS /opt/wpilib CACHE PATH "WPILib directory.")
set(CTRE_LIBS /opt/ctre CACHE PATH "CTRE directory.")
set(NAVX_LIBS /opt/navx CACHE PATH "NavX directory.")

set(CMAKE_INSTALL_PREFIX /opt/sidewinder CACHE PATH "Sidewinder installation directory." FORCE)

set(VERSION_MAJOR 17)
set(VERSION_MINOR 4)
set(VERSION_PATCH 0)
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

include_directories(SYSTEM
  "${WPI_LIBS}/include"
  "${CTRE_LIBS}/include"
  "${NAVX_LIBS}/include"
  "${CMAKE_SOURCE_DIR}/vendor/include"
  "${PROJECT_BINARY_DIR}/src"
)

link_directories(
  "${WPI_LIBS}/lib"
  "${CTRE_LIBS}/lib"
  "${NAVX_LIBS}/lib"
)

add_subdirectory(src)
add_subdirectory(examples)
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
