#ifndef DEADEYE_DEADEYE_INC_
#define DEADEYE_DEADEYE_INC_

#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"

#include "deadeye/camera.h"
#include "deadeye/config.h"
#include "deadeye/robot.h"

namespace deadeye {
struct BottomCorners {
  cv::Point left, right;
};

struct AimPoint {
  double center, dist_inches;
};

class Deadeye {
 private:
  std::unique_ptr<deadeye::Robot> robot_;
  std::unique_ptr<deadeye::Camera> camera_;
  cv::Scalar upper_, lower_;

  std::unique_ptr<BottomCorners> FindBottomCorners(
      const std::vector<cv::Point>& target_contour);

  std::unique_ptr<AimPoint> FindAimPoint(
      std::unique_ptr<BottomCorners> corners);

 public:
  // note: these are only useful for development and debugging
  cv::Mat hsv_frame, in_range_frame, dilated_frame, eroded_frame;

  Deadeye(std::shared_ptr<deadeye::Config> config);
  virtual ~Deadeye();
  std::vector<cv::Point> TargetContour(const cv::Mat& frame);
  void Start();
  void Stop();
};
} /* deadeye */

#endif /* end of include guard: DEADEYE_DEADEYE_INC_ */
