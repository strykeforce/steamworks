set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

include_directories(
  "${WPI_LIBS}/include"
  "${CTRE_LIBS}/include"
  "${CMAKE_SOURCE_DIR}/lib"
)

link_directories(
  "${WPI_LIBS}/lib"
  "${CTRE_LIBS}/lib"
)

add_executable(BDC_Comm
  Main.cpp
)

target_link_libraries(BDC_Comm
  grapher
  CTRLib
  CTRLibDriver
  wpi
)
