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
 public:
  typedef std::function<bool(const cv::Mat& img)> ImageCallbackFunc;

 private:
  std::unique_ptr<deadeye::Robot> robot_;
  std::unique_ptr<deadeye::Camera> camera_;
  cv::Scalar upper_, lower_;
  cv::Mat hsv_frame_, in_range_frame_, dilated_frame_, eroded_frame_;

  std::unique_ptr<BottomCorners> FindBottomCorners(
      const std::vector<cv::Point>& target_contour);

  std::unique_ptr<AimPoint> FindAimPoint(
      std::unique_ptr<BottomCorners> corners);

  ImageCallbackFunc frameCallback_, maskCallback_;

 public:

  Deadeye(std::shared_ptr<deadeye::Config> config);
  virtual ~Deadeye();
  std::vector<cv::Point> TargetContour(const cv::Mat& frame);
  void Start();
  void Stop();
  void SetFrameCallback(ImageCallbackFunc func);
  void SetMaskCallback(ImageCallbackFunc func);
};
} /* deadeye */

#endif /* end of include guard: DEADEYE_DEADEYE_INC_ */
