#ifndef DEADEYE_DEADEYE_INC_
#define DEADEYE_DEADEYE_INC_

#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"

#include "config.h"

namespace deadeye {
class Deadeye {
 private:
  cv::Scalar upper_, lower_;
  // int min_perimeter_;

 public:
  // note: these are only useful for development and debugging
  cv::Mat hsv_frame, in_range_frame, dilated_frame, eroded_frame;

  Deadeye(std::shared_ptr<deadeye::Config> config);
  virtual ~Deadeye();
  std::vector<cv::Point> TargetContour(const cv::Mat& frame);
};
} /* deadeye */

#endif /* end of include guard: DEADEYE_DEADEYE_INC_ */
