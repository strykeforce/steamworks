#pragma once

#include <utility>
#include <cpptoml/cpptoml.h>
#include <spdlog/spdlog.h>
#include <opencv2/opencv.hpp>

#include "gear_frame.h"

namespace deadeye {

class GearCamera {
 public:
  GearCamera(std::shared_ptr<cpptoml::table> config);
  virtual ~GearCamera();
  GearCamera(const GearCamera&) = delete;
  GearCamera& operator=(const GearCamera&) = delete;

  void Connect();
  void Disconnect();
  bool IsConnected();
  void StartCapture();
  void StopCapture();
  std::tuple<int, int> ProcessFrame();
  void DisplayFrame();

 private:
  std::shared_ptr<spdlog::logger> logger_;

  // hardware
  std::unique_ptr<cv::VideoCapture> camera_{nullptr};
  bool connected_, capture_started_, has_gui_;
  cv::Mat frame_;
  double exposure_;

  static const std::tuple<int, int> PROCESS_ERROR;
  // frame processing
  GearFrame frame_process_;
};

} /* deadeye */
