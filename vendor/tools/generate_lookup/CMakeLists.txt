set(GENERATE_CMD "${CMAKE_CURRENT_LIST_DIR}/generate.py")
set(BOILER_LOOKUP_H "${PROJECT_BINARY_DIR}/src/boiler_target_data.h")

add_custom_command(
  OUTPUT ${BOILER_LOOKUP_H}
  COMMAND ${GENERATE_CMD} --input ${BOILER_LOOKUP_INPUT} > ${BOILER_LOOKUP_H}
  DEPENDS ${BOILER_LOOKUP_INPUT}
  VERBATIM
)

add_custom_target(generate_lookup DEPENDS ${BOILER_LOOKUP_H})
