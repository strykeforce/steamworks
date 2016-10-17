CUDA_SRCS = glob([
  "usr/local/cuda-7.0/targets/armv7-linux-gnueabihf/lib/libcufft.so*",
  "usr/local/cuda-7.0/targets/armv7-linux-gnueabihf/lib/libnpps.so*",
  "usr/local/cuda-7.0/targets/armv7-linux-gnueabihf/lib/libnppi.so*",
  "usr/local/cuda-7.0/targets/armv7-linux-gnueabihf/lib/libnppc.so*",
  "usr/local/cuda-7.0/targets/armv7-linux-gnueabihf/lib/libcudart.so*",
])

STDLIB_SRCS = glob([
  "lib/arm-linux-gnueabihf/lib*.so*",
  "usr/lib/libtbb.so*",
  "usr/lib/arm-linux-gnueabihf/lib*.so*",
  "usr/lib/arm-linux-gnueabihf/neon/vfp/lib*.so*",
  "usr/lib/arm-linux-gnueabihf/x264-10bit/libx264.so*",
  "usr/lib/arm-linux-gnueabihf/oxide-qt/lib*.so*",
], exclude=[
  "usr/lib/arm-linux-gnueabihf/*.so.1d",
  "usr/lib/arm-linux-gnueabihf/libc.so",
  "usr/lib/arm-linux-gnueabihf/libpthread.so",
  ])

OPENCV_SRCS = glob([
  "usr/lib/libopencv_*",
])

cc_library(
    name = "lib",
    srcs = OPENCV_SRCS + CUDA_SRCS + STDLIB_SRCS,
    hdrs = glob(["usr/include/opencv2/**"]),
    includes = ["usr/include"],
    visibility = ["//visibility:public"],
    linkstatic = 1,
)
