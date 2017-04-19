#pragma once

#include <vector>

#include <cpptoml/cpptoml.h>
#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>

namespace deadeye {

class BoilerFrame {
 public:
  cv::Mat hsv, blur, mask;
  std::vector<cv::Point> upper_contour, lower_contour;
  cv::Rect upper_rect, lower_rect;
  int azimuth_error, centerline_error;

  BoilerFrame(std::shared_ptr<cpptoml::table> config);
  virtual ~BoilerFrame() = default;

  bool FindTargets(const cv::Mat& frame);

 private:
  std::shared_ptr<spdlog::logger> logger_;
  cv::Scalar hsv_lower_, hsv_upper_;
  double min_arc_length_;

  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
};

} /* deadeye */
