#pragma once

#include <utility>

#include <flycapture/FlyCapture2.h>
#include <cpptoml/cpptoml.h>
#include <spdlog/spdlog.h>
#include <opencv2/opencv.hpp>

#include "boiler_frame.h"

namespace fc = FlyCapture2;

namespace deadeye {

class BoilerCamera {
 public:
  BoilerCamera(std::shared_ptr<cpptoml::table> config);
  virtual ~BoilerCamera();
  BoilerCamera(const BoilerCamera&) = delete;
  BoilerCamera& operator=(const BoilerCamera&) = delete;

  void Connect();
  void Disconnect();
  bool IsConnected();
  void StartCapture();
  void StopCapture();
  bool ProcessFrame(int& azimuth_error, int& centerline_error);
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
  BoilerFrame frame_process_;
};

} /* deadeye */
