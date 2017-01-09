set(GENERATED_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/src)

add_subdirectory(generate_version)
add_subdirectory(generate_default_config)
add_subdirectory(deploy)
