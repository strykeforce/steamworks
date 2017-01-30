target_sources(sidewinder
  PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/CodeUtils.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/JerrysGrapher.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/SocketHandler.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/TerminalEscapes.cpp"
  PUBLIC
  "${CMAKE_CURRENT_LIST_DIR}/CodeUtils.h"
  "${CMAKE_CURRENT_LIST_DIR}/HostToObject.h"
  "${CMAKE_CURRENT_LIST_DIR}/NetworkPorts.h"
  "${CMAKE_CURRENT_LIST_DIR}/ObjectToGrapher.h"
  "${CMAKE_CURRENT_LIST_DIR}/SocketHandler.h"
  "${CMAKE_CURRENT_LIST_DIR}/TerminalEscapes.h"
)

target_include_directories(sidewinder PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
