cmake_minimum_required(VERSION 3.2)
project(Sidewinder)

set(WPI_LIBS /opt/wpilib CACHE PATH "WPILib directory.")
set(CMAKE_INSTALL_PREFIX /opt/sidewinder CACHE PATH "Sidewinder installation directory." FORCE)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)
set(VERSION_META "-alpha")

set(CMAKE_CXX_COMPILER /usr/bin/arm-frc-linux-gnueabi-g++)
set(CMAKE_LD           /usr/bin/arm-frc-linux-gnueabi-ld)
set(CMAKE_AR           /usr/bin/arm-frc-linux-gnueabi-ar)
set(CMAKE_RANLIB       /usr/bin/arm-frc-linux-gnueabi-ranlib)
set(CMAKE_OBJCOPY      /usr/bin/arm-frc-linux-gnueabi-objcopy)
set(CMAKE_OBJCOPY      /usr/bin/arm-frc-linux-gnueabi-objdump)
set(CMAKE_NM           /usr/bin/arm-frc-linux-gnueabi-nm)
set(CMAKE_STRIP        /usr/bin/arm-frc-linux-gnueabi-strip)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -fstrict-aliasing -std=c++1y")


set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

include_directories(SYSTEM
  "${WPI_LIBS}/include"
  "${CMAKE_SOURCE_DIR}/vendor/include"
)

link_directories(
  "${WPI_LIBS}/lib"
)

# add_subdirectory(vendor/tools)
add_subdirectory(src)
add_subdirectory(examples)
