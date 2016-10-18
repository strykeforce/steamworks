#ifndef DEADEYE_CAMERA_INC_
#define DEADEYE_CAMERA_INC_

#include <opencv2/opencv.hpp>

#include "deadeye/config.h"

namespace deadeye {
class Camera {
 private:
  std::unique_ptr<cv::VideoCapture> camera_;

  void ConfigCameraV4L2();

 public:
  int framewidth;
  Camera(std::shared_ptr<deadeye::Config> config);
  virtual ~Camera();
  bool Read(cv::Mat& frame);
};
} /* deadeye */
#endif /* end of include guard: DEADEYE_CAMERA_INC_ */
