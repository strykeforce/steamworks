set(STRONGHOLD_SHARED_LIBS ../../../Shared_Libs)
set(WPI_LIBS ../../../../2017_Static/Robot_Libs)
set(NAVX_LIBS ../../../../2017_Static/NavX_Libs)

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

include_directories(
  "${PROJECT_SOURCE_DIR}/include"
  "${STRONGHOLD_SHARED_LIBS}/Headers"
  "${WPI_LIBS}/include"
  "${NAVX_LIBS}/include"
)

link_directories(
  "${WPI_LIBS}/lib"
  "${NAVX_LIBS}/lib"
  "${STRONGHOLD_SHARED_LIBS}/Compiled_RIO"
)

add_executable(BDC_Comm
  Main.cpp
)

target_link_libraries(BDC_Comm
  "rio"
  "navx_frc_cpp"
  "wpi"
)
