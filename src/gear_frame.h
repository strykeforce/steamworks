#pragma once

#include <vector>

#include <cpptoml/cpptoml.h>
#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>

namespace deadeye {

class GearFrame {
 public:
  cv::Mat hsv, blur, mask;
  std::vector<cv::Point> left_contour, right_contour;
  cv::Rect left_rect, right_rect;
  int azimuth_error, target_separation;

  GearFrame(std::shared_ptr<cpptoml::table> config);
  virtual ~GearFrame() = default;

  bool FindTargets(const cv::Mat& frame);

 private:
  std::shared_ptr<spdlog::logger> logger_;
  cv::Scalar hsv_lower_, hsv_upper_;
  double min_arc_length_;
};

} /* deadeye */
