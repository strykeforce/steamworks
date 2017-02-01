#pragma once

#include <utility>

#include "flycapture/FlyCapture2.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "frame.h"

namespace fc = FlyCapture2;

namespace deadeye {

class Camera {
 public:
  Camera(std::shared_ptr<cpptoml::table> config);
  virtual ~Camera();
  Camera(const Camera&) = delete;
  Camera& operator=(const Camera&) = delete;

  void Connect();
  void Disconnect();
  void StartCapture();
  void StopCapture();
  std::tuple<int, int> ProcessFrame();
  void DisplayFrame();

 private:
  std::shared_ptr<spdlog::logger> logger_;

  // hardware
  bool connected_, capture_started_, has_gui_;
  fc::Camera camera_;
  fc::Image raw_image_, rgb_image_;
  cv::Mat frame_;
  int width_;
  int height_;
  double exposure_;

  static const std::tuple<int, int> PROCESS_ERROR;
  // frame processing
  Frame frame_process_;
};

} /* deadeye */
