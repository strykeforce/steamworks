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
  bool has_left_target;
  int azimuth_error;
  int target_height;
  int target_width;

  GearFrame(std::shared_ptr<cpptoml::table> config);
  virtual ~GearFrame() = default;

  bool FindTargets(const cv::Mat& frame);
  int GetAzimuthOffset();

 private:
  std::shared_ptr<spdlog::logger> logger_;
  cv::Scalar hsv_lower_, hsv_upper_;
  double min_arc_length_;
  int azimuth_offset_;
};

} /* deadeye */
