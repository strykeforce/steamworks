CUDA_SRCS = glob([
  "usr/local/cuda-7.0/targets/armv7-linux-gnueabihf/lib/libcufft.so*",
  "usr/local/cuda-7.0/targets/armv7-linux-gnueabihf/lib/libnpps.so*",
  "usr/local/cuda-7.0/targets/armv7-linux-gnueabihf/lib/libnppi.so*",
  "usr/local/cuda-7.0/targets/armv7-linux-gnueabihf/lib/libnppc.so*",
  "usr/local/cuda-7.0/targets/armv7-linux-gnueabihf/lib/libcudart.so*",
])

STDLIB_SRCS = glob([
  "usr/lib/libtbb.so*",
  # "usr/lib/arm-linux-gnueabihf/libjpeg.so*",
  # "usr/lib/arm-linux-gnueabihf/libtiff.so*",
  # "usr/lib/arm-linux-gnueabihf/libgtk-x11-2.0.so.0*",
  # "usr/lib/arm-linux-gnueabihf/libgdk-x11-2.0.so.0*",
  # "usr/lib/arm-linux-gnueabihf/libgobject-2.0.so.0*",
  # "usr/lib/arm-linux-gnueabihf/libgmodule-2.0.so.0*",
  # "usr/lib/arm-linux-gnueabihf/libpangocairo-1.0.so.0*",
  # "usr/lib/arm-linux-gnueabihf/libatk-1.0.so.0*",
  # "usr/lib/arm-linux-gnueabihf/libgdk_pixbuf-2.0.so.0*",
  # "usr/lib/arm-linux-gnueabihf/libgio-2.0.so.0*",
  # "usr/lib/arm-linux-gnueabihf/libpangoft2-1.0.so.0*",
  # "usr/lib/arm-linux-gnueabihf/libpango-1.0.so.0*",
  # "usr/lib/arm-linux-gnueabihf/libjasper.so*",
  # "usr/lib/arm-linux-gnueabihf/libavcodec.so*",
  # "usr/lib/arm-linux-gnueabihf/neon/vfp/libavcodec.so*",
  # "usr/lib/arm-linux-gnueabihf/libavformat.so*",
  # "usr/lib/arm-linux-gnueabihf/neon/vfp/libavformat.so*",
  # "usr/lib/arm-linux-gnueabihf/libavutil.so*",
  # "usr/lib/arm-linux-gnueabihf/neon/vfp/libavutil.so*",
  # "usr/lib/arm-linux-gnueabihf/libavutil.so*",
  # "usr/lib/arm-linux-gnueabihf/neon/vfp/libavutil.so*",
  # "usr/lib/arm-linux-gnueabihf/libswscale.so*",
  # "usr/lib/arm-linux-gnueabihf/neon/vfp/libswscale.so*",
  # "usr/lib/arm-linux-gnueabihf/libjbig.so*",
  # "usr/lib/arm-linux-gnueabihf/libX*.so*",
  # "usr/lib/arm-linux-gnueabihf/libcairo.so*",
  # "usr/lib/arm-linux-gnueabihf/libfontconfig.so*",
  # "usr/lib/arm-linux-gnueabihf/libffi.so*",
  # "usr/lib/arm-linux-gnueabihf/libxvidcore.so*",
  # "usr/lib/arm-linux-gnueabihf/libx264.so*",
  # "usr/lib/arm-linux-gnueabihf/libvpx.so*",
  # "usr/lib/arm-linux-gnueabihf/vfp/neon/libvpx.so*",
  # "usr/lib/arm-linux-gnueabihf/libvorbis*.so*",
  # "usr/lib/arm-linux-gnueabihf/libtheora*.so*",
  "usr/lib/arm-linux-gnueabihf/lib*.so*",
  "usr/lib/arm-linux-gnueabihf/neon/vfp/lib*.so*",
  "usr/lib/arm-linux-gnueabihf/x264-10bit/libx264.so*",
  # "usr/lib/arm-linux-gnueabihf/libX11.so*",
  # "usr/lib/arm-linux-gnueabihf/libXfixes.so*",
  # "usr/lib/arm-linux-gnueabihf/libXrender.so*",
  # "usr/lib/arm-linux-gnueabihf/libXinerama.so*",
  # "usr/lib/arm-linux-gnueabihf/libXi.so*",
  # "usr/lib/arm-linux-gnueabihf/libXrandr.so*",
  # "usr/lib/arm-linux-gnueabihf/libXcursor.so*",

  # "lib/arm-linux-gnueabihf/liblzma.so*",
  # "lib/arm-linux-gnueabihf/libglib-2.0.so.0*",
  # "lib/arm-linux-gnueabihf/libpng12.so*",
  # "lib/arm-linux-gnueabihf/librt*",
  # "lib/arm-linux-gnueabihf/libpthread*",
  # "lib/arm-linux-gnueabihf/libm.so*",
  # "lib/arm-linux-gnueabihf/libm-*",
  # "lib/arm-linux-gnueabihf/libdl*",
  # "lib/arm-linux-gnueabihf/libz.so*",
  # "lib/arm-linux-gnueabihf/libpcre.so*",
  "usr/lib/arm-linux-gnueabihf/oxide-qt/lib*.so*",
  "lib/arm-linux-gnueabihf/lib*.so*",
], exclude=[
  "usr/lib/arm-linux-gnueabihf/*.so.1d",
  "usr/lib/arm-linux-gnueabihf/libc.so",
  "usr/lib/arm-linux-gnueabihf/libpthread.so",
  ])

OPENCV_SRCS = glob([
  "usr/lib/libopencv_*",
  # "usr/lib/libopencv_contrib.so*",
  # "usr/lib/libopencv_core.so*",
  # "usr/lib/libopencv_features2d.so*",
  # "usr/lib/libopencv_flann.so*",
  # "usr/lib/libopencv_gpu.so*",
  # "usr/lib/libopencv_highgui.so*",
  # "usr/lib/libopencv_imgproc.so*",
  # "usr/lib/libopencv_legacy.so*",
  # "usr/lib/libopencv_ml.so*",
  # "usr/lib/libopencv_objdetect.so*",
  # "usr/lib/libopencv_photo.so*",
  # "usr/lib/libopencv_stitching.so*",
  # "usr/lib/libopencv_superres.so*",
  # "usr/lib/libopencv_ts.so*",
  # "usr/lib/libopencv_video.so*",
  # "usr/lib/libopencv_videostab.so*",
  # "usr/lib/libopencv_detection_based_tracker.so*",
  # "usr/lib/libopencv_esm_panorama.so*",
  # "usr/lib/libopencv_facedetect.so*",
  # "usr/lib/libopencv_imuvstab.so*",
  # "usr/lib/libopencv_tegra.so*",
  # "usr/lib/libopencv_vstab.so*",
])

cc_library(
    name = "lib",
    # srcs = CUDA_SRCS + STDLIB_SRCS + OPENCV_SOURCES,
    srcs = OPENCV_SRCS + CUDA_SRCS + STDLIB_SRCS,
    hdrs = glob(["usr/include/opencv2/**"]),
    includes = ["usr/include"],
    visibility = ["//visibility:public"],
    linkopts = [
    # "--sysroot=external/opencv_jetson_cross",
    # "-Lexternal/opencv_jetson_cross/lib/arm-linux-gnueabihf",
    # "-Lexternal/opencv_jetson_cross/usr/lib/arm-linux-gnueabihf",
    # "-Lexternal/opencv_jetson_cross/usr/lib/arm-linux-gnueabihf/tegra",
    "-Lexternal/opencv_jetson_cross/usr/local/cuda-7.0/targets/armv7-linux-gnueabihf/lib",
    ],
    linkstatic = 1,
)