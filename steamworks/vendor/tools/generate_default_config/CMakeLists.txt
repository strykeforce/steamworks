set(DEFAULT_CONFIG_INPUT "${CMAKE_SOURCE_DIR}/conf/default_config.toml")
set(GENERATE_CMD "${CMAKE_SOURCE_DIR}/vendor/tools/generate_default_config/generate.py")

add_custom_command(
  OUTPUT ${DEFAULT_CONFIG_H}
  COMMAND ${GENERATE_CMD} --input ${DEFAULT_CONFIG_INPUT} > ${DEFAULT_CONFIG_H}
  DEPENDS ${DEFAULT_CONFIG_INPUT}
  VERBATIM
)

add_custom_target(generate_default_config DEPENDS ${DEFAULT_CONFIG_H})
