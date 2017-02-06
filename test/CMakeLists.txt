cmake_minimum_required(VERSION 3.2)
project(deadeye-test)

set(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
find_package(OpenCV  2.4 REQUIRED)

# BOILER_LOOKUP_INPUT is used by vendor/tools/generate_lookup. It is set here so
# we can set it separately in project CMakeLists.txt. It needs to be set prior
# to adding the vendor subdirectory.
set(BOILER_LOOKUP_INPUT "${CMAKE_SOURCE_DIR}/../conf/steamworks_boiler_data.xlsx")
add_subdirectory(
  ${CMAKE_CURRENT_SOURCE_DIR}/../vendor
  ${CMAKE_CURRENT_BINARY_DIR}/vendor
)
add_definitions(-DDEADEYE_TEST)

add_executable(test-runner
  test-runner.cc
  deadeye_test.cc
  sentence_test.cc
  parser_test.cc
  boiler_test.cc
  checksum_test.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/deadeye.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/link.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/camera.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/frame.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/link/checksum.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/link/sentence.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/link/parser.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/../src/link/boiler.cc
)

set(INCLUDE_DIRS
${CMAKE_CURRENT_SOURCE_DIR}/../vendor/include
${CMAKE_CURRENT_SOURCE_DIR}/../src
${CMAKE_CURRENT_SOURCE_DIR}/../vendor/serial/include
"${PROJECT_BINARY_DIR}/src"
)

find_file(HAVE_FLYCAPTURE_OPT FlyCapture2.h
PATHS /opt/flycapture/include/flycapture NO_DEFAULT_PATH)

if(HAVE_FLYCAPTURE_OPT)
  list(APPEND INCLUDE_DIRS /opt/flycapture/include)
endif(HAVE_FLYCAPTURE_OPT)

target_include_directories(test-runner PUBLIC ${INCLUDE_DIRS})

target_link_libraries(test-runner ${OpenCV_LIBS} flycapture serial)

set_target_properties(test-runner
  PROPERTIES
  CXX_STANDARD 11
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED YES
)

add_dependencies(test-runner generate_lookup)
