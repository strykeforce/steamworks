#ifndef DEADEYE_FRAME_PROCESSOR_INC_
#define DEADEYE_FRAME_PROCESSOR_INC_

#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"

#include "config.h"

namespace deadeye {
class FrameProcessor {
 private:
  cv::Scalar upper_, lower_;
  int min_perimeter_;

 public:
  FrameProcessor(std::shared_ptr<deadeye::Config> config);
  virtual ~FrameProcessor();
  std::shared_ptr<std::vector<cv::Point>> TargetContour(const cv::Mat& frame);
};
} /* deadeye */

#endif /* end of include guard: DEADEYE_FRAME_PROCESSOR_INC_ */
