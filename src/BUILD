# Description: deadeye robot vision system apps

DEADEYE_DEPS = ["@opencv//:lib", "//lib:deadeye-core", "@spdlog//:lib"]

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
