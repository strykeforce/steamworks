# Description: core library for deadeye robot vision system

cc_library(
  name = "deadeye-core",
  srcs = glob(["*.cc"]) + ["constants.h"],
  hdrs = glob(["*.h"], exclude = ["constants.h"]),
  deps = [
    "@opencv//:lib",
    "@spdlog//:lib",
    "@cpptoml//:lib",
  ],
  visibility = ["//src:__pkg__"],
)
