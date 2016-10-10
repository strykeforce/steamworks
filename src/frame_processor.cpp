#include "frame_processor.h"

#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"

#include "config.h"

namespace spd = spdlog;

namespace deadeye {

FrameProcessor::FrameProcessor(std::shared_ptr<deadeye::Config> config)
    : upper_(config->range_upper), lower_(config->range_lower) {}

FrameProcessor::~FrameProcessor() {}

std::shared_ptr<std::vector<cv::Point>> FrameProcessor::TargetContour(
    const cv::Mat& frame) {
  auto console = spd::get("console");

  // TODO: get rid of extra matrix variables
  // TODO: do this in HSV
  // TODO: get rid of morph. closing
  cv::Mat in_range_frame, dilated_frame, eroded_frame;
  cv::inRange(frame, lower_, upper_, in_range_frame);
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
    return nullptr;
  }

  if (cv::arcLength(contours[best_index], true) < 250) {
    return nullptr;
  }

  return std::shared_ptr<std::vector<cv::Point>>(&contours[best_index]);
}
} /* deadeye */
