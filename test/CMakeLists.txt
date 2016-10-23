set(testing_linked_libs deadeye_core catch)
file(GLOB test_sources "*.cc" )

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/testdata/testdata.h.in"
  "${PROJECT_BINARY_DIR}/testdata.h"
)
include_directories("${PROJECT_BINARY_DIR}")

add_executable(test-runner ${test_sources})
target_link_libraries(test-runner ${testing_linked_libs})
set_target_properties(test-runner PROPERTIES
  CXX_STANDARD 11
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED YES)

add_test(NAME test-runner-cmake COMMAND test-runner)
