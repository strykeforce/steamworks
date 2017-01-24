#pragma once

#include <memory>
#include "cpptoml/cpptoml.h"
#include "opencv2/opencv.hpp"

namespace deadeye {

class Frame {
 private:
 public:
  std::vector<cv::Point> upper_contour, lower_contour;
  Frame(std::shared_ptr<cpptoml::table> config);
  virtual ~Frame() = default;

  bool FindTargets(const cv::Mat& frame);
};

} /* deadeye */
