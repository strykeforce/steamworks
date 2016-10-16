workspace(name = "org_strykeforce_deadeye")

new_local_repository(
  name = "opencv",
  path = "/usr/local/opt/opencv",
  build_file_content = """
cc_library(
    name = "lib",
    srcs = glob(["lib/*.dylib"]),
    hdrs = glob(["include/**/*.hpp"]),
    includes = ["include"],
    visibility = ["//visibility:public"],
    linkstatic = 1,
)
""",
)

new_http_archive(
  name = "spdlog",
  url = "https://github.com/gabime/spdlog/archive/v0.11.0.tar.gz",
  strip_prefix = "spdlog-0.11.0",
  build_file_content = """
cc_library(
    name = "lib",
    hdrs = glob(["include/**"]),
    includes = ["include"],
    visibility = ["//visibility:public"],
)
""",
)

new_git_repository(
  name = "cpptoml",
  remote = "https://github.com/skystrife/cpptoml.git",
  commit = "b0a6ac46c2470e55ad809409f42273758e6be10f",
  build_file_content = """
cc_library(
  name = "lib",
  hdrs = ["include/cpptoml.h"],
  includes = ["include"],
  visibility = ["//visibility:public"],
)
""",
)
