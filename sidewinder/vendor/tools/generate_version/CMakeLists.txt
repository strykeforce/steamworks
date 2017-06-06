add_executable(version_gen generate.c)

target_compile_options(version_gen PRIVATE "-Wno-unused-result")
