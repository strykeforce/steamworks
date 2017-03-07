set(GENERATED_OUTPUT_DIR ${PROJECT_BINARY_DIR}/src)

add_subdirectory(generate_version)
add_subdirectory(generate_default_config)
add_subdirectory(deploy)
add_subdirectory(generate_lookup)
