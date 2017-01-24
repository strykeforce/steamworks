#include "frame.h"

#include "cpptoml/cpptoml.h"
#include "opencv2/opencv.hpp"
#include <memory>

using namespace deadeye;

Frame::Frame(std::shared_ptr<cpptoml::table> config) {}

bool Frame::FindTargets(const cv::Mat& frame) {
  cv::Mat hsl, blur, mask;
  cv::cvtColor(frame, hsl, CV_BGR2HSV);
  cv::blur(hsl, blur, cv::Size(2, 2));
  cv::inRange(blur, cv::Scalar{80, 100, 100}, cv::Scalar{100, 255, 255}, mask);

  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(mask.clone(), contours, CV_RETR_LIST,
                   CV_CHAIN_APPROX_SIMPLE);

  double arc_length = 0.0;
  for (const auto& c : contours) {
    auto candidate = cv::arcLength(c, true);
    if (candidate > arc_length) {
      lower_contour = upper_contour;
      upper_contour = c;
      arc_length = candidate;
    }
  }
  return arc_length > 250.0;
}
