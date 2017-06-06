set(GENERATE_CMD "${CMAKE_CURRENT_LIST_DIR}/generate.py")
set(SHOOTER_LOOKUP_H "${PROJECT_BINARY_DIR}/src/shooter_target_data.h")
set(SHOOTER_LOOKUP_INPUT "${CMAKE_SOURCE_DIR}/conf/shooter_data.xlsx")

add_custom_command(
  OUTPUT ${SHOOTER_LOOKUP_H}
  COMMAND ${GENERATE_CMD} --input ${SHOOTER_LOOKUP_INPUT} > ${SHOOTER_LOOKUP_H}
  DEPENDS ${SHOOTER_LOOKUP_INPUT}
  VERBATIM
)

add_custom_target(generate_lookup DEPENDS ${SHOOTER_LOOKUP_H})
