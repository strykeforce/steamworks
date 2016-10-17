# Description: core library for deadeye robot vision system

DEADEYE_OPENCV_DEP = select({
  "//tools/crosstool:jetson_cross_compile": ["@opencv_jetson_cross//:lib"],
  "//conditions:default": ["@opencv_darwin//:lib"],
  })

DEADEYE_DEPS = DEADEYE_OPENCV_DEP + ["@spdlog//:lib", "@cpptoml//:lib"]


cc_library(
  name = "lib",
  srcs = glob(["*.cc"]) + ["constants.h"],
  hdrs = glob(["*.h"], exclude = ["constants.h"]),
  deps = DEADEYE_DEPS,
  visibility = ["//main:__pkg__"],
  linkstatic = 1,
)
