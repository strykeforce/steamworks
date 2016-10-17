# Description: deadeye robot vision system apps

DEADEYE_OPENCV_DEP = select({
  "//tools/crosstool:jetson_cross_compile": ["@opencv_jetson_cross//:lib"],
  "//conditions:default": ["@opencv_darwin//:lib"],
  })

DEADEYE_DEPS = DEADEYE_OPENCV_DEP + ["//deadeye", "@spdlog//:lib"]

cc_binary (
  name = "pipeline",
  srcs = ["pipeline.cc"],
  deps = DEADEYE_DEPS,
)

cc_binary (
  name = "deadeyed",
  srcs = ["deadeyed.cc"],
  deps = DEADEYE_DEPS,
)

cc_binary (
  name = "deadeye-snap",
  srcs = ["snap.cc"],
  deps = DEADEYE_DEPS,
)

cc_binary (
  name = "hello",
  srcs = ["hello.cc"],
)
