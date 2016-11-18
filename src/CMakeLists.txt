set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

include_directories(
  "${STRONGHOLD_SHARED_LIBS}/Headers"
  "${WPI_LIBS}/include"
)

link_directories(
  "${WPI_LIBS}/lib"
  "${STRONGHOLD_SHARED_LIBS}/Compiled_RIO"
)

add_executable(BDC_Comm
  Main.cpp
)

target_include_directories(BDC_Comm
  PUBLIC
  "${CMAKE_SOURCE_DIR}/lib"
)

target_link_libraries(BDC_Comm
  grapher
  wpi
)
