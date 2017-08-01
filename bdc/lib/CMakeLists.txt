include_directories(
  "${WPI_LIBS}/include"
  "${CTRE_LIBS}/include"
  "${CMAKE_SOURCE_DIR}/lib"
)

add_library(grapher
CodeUtils.cpp
JerrysGrapher.cpp
SocketHandler.cpp
TerminalEscapes.cpp
RIOUtils.cpp
)
