#include "deadeye/deadeye.h"

#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"

#include "deadeye/config.h"

namespace spd = spdlog;

namespace deadeye {

Deadeye::Deadeye(std::shared_ptr<deadeye::Config> config) {
  auto console = spd::get("console");
  auto target = config->GetTable("target");

  // TODO: validate returned arrays
  auto hsv_l = target->get_array_of<int64_t>("HSV_lower");
  auto hsv_u = target->get_array_of<int64_t>("HSV_upper");

  lower_ = cv::Scalar((*hsv_l)[0], (*hsv_l)[1], (*hsv_l)[2]);
  upper_ = cv::Scalar((*hsv_u)[0], (*hsv_u)[1], (*hsv_u)[2]);

  console->debug("range_lower = ({}, {}, {})", lower_[0], lower_[1], lower_[2]);
  console->debug("range_upper = ({}, {}, {})", upper_[0], upper_[1], upper_[2]);
}

Deadeye::~Deadeye() {}

std::vector<cv::Point> Deadeye::TargetContour(const cv::Mat& frame) {
  auto console = spd::get("console");

  // TODO: get rid of extra matrix variables
  // TODO: get rid of morph. closing
  cv::cvtColor(frame, hsv_frame, CV_BGR2HSV);
  cv::inRange(hsv_frame, lower_, upper_, in_range_frame);
  cv::dilate(in_range_frame, dilated_frame, cv::Mat(), cv::Point(-1, -1), 2, 1,
             1);
  cv::erode(dilated_frame, eroded_frame, cv::Mat(), cv::Point(-1, -1), 1, 1, 1);
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(eroded_frame.clone(), contours, CV_RETR_LIST,
                   CV_CHAIN_APPROX_SIMPLE);
  console->debug("counts.size() == {}", contours.size());
  int best_index = -1;
  double best_size = 0;

  // test for target is to find longest contour perimeter
  // TODO: use cv::arcLength
  for (uint i = 0; i < contours.size(); i++) {
    console->debug("counts[{}].size() == {}", i, contours[i].size());
    double current_size = 0;
    for (uint KK = 0; KK < contours[i].size(); KK++) {
      int index2 = (KK + 1) % contours[i].size();
      // accumulate distance between current and next point in contour
      current_size += sqrt(pow(contours[i][index2].x - contours[i][KK].x, 2) +
                           pow(contours[i][index2].y - contours[i][KK].y, 2));
    }
    if (best_size < current_size) {
      best_size = current_size;
      best_index = i;
    }
  }  // done finding longest target perimeter

  console->debug("best contour is contours[{}].size() == {}", best_index,
                 best_size);

  if (best_index == -1) {
    return {};
  }

  if (cv::arcLength(contours[best_index], true) < 250) {
    return {};
  }

  return contours[best_index];
}
} /* deadeye */
