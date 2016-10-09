#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"

#include "config.h"
#include "constants.h"
#include "message.h"
#include "nvidia_utils.h"
#include "target.h"

namespace spd = spdlog;

void start(std::shared_ptr<deadeye::Config> config) {
  auto console = spd::get("console");
  auto message = new deadeye::Message(config);
  float payload[3];

  ConfigCameraV4L2();

  cv::VideoCapture vcap(0);
  vcap.set(CV_CAP_PROP_FRAME_WIDTH, kFrameWidth);
  vcap.set(CV_CAP_PROP_FRAME_HEIGHT, kFrameHeight);
  vcap.set(CV_CAP_PROP_BRIGHTNESS, 0.0);
  cv::Mat m1, m2, m3, m4, m5;

  for (;;) {
    vcap.read(m1);
    cv::inRange(m1, config->range_lower, config->range_upper, m2);
    cv::dilate(m2, m3, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
    cv::erode(m3, m4, cv::Mat(), cv::Point(-1, -1), 1, 1, 1);
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(m4.clone(), contours, CV_RETR_LIST,
                     CV_CHAIN_APPROX_SIMPLE);
    console->debug("counts.size() == {}", contours.size());
    int best_index = -1;
    double best_size = 0;

    // test for target is to find longest contour perimeter
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
      console->debug("contours[{}].size() == {}", i, current_size);
    }  // done finding longest target perimeter

    console->debug("contours[{}].size() == {}", best_index, best_size);

    // punt on this frame if not large enough perimeter
    if (best_size < 250) {
      payload[0] = 0.0;
      payload[1] = 0.0;
      payload[2] = 42.0;
      message->send(payload);
      continue;
    }

    // #define points contours[best_index]
    cv::Point leftest = contours[best_index][0], rightest = leftest,
              bottomest = leftest, topest = leftest;
    int leftest_index = 0, rightest_index = leftest_index,
        bottomest_index = leftest_index;

    // find left, top, right and bottom-most contour points
    for (uint i = 1; i < contours[best_index].size(); i++) {
      if (contours[best_index][i].x < leftest.x) {
        leftest = contours[best_index][i];
        leftest_index = i;
      }
      if (contours[best_index][i].x > rightest.x) {
        rightest = contours[best_index][i];
        rightest_index = i;
      }
      if (contours[best_index][i].y > bottomest.y) {
        bottomest = contours[best_index][i];
        bottomest_index = i;
      }
      if (contours[best_index][i].y < topest.y) {
        topest = contours[best_index][i];
      }
    }

    cv::Point left_a = corner_point(contours[best_index], leftest_index, 90, 1);
    cv::Point left_b =
        corner_point(contours[best_index], leftest_index, 270, -1);
    cv::Point right_a =
        corner_point(contours[best_index], rightest_index, 270, 1);
    cv::Point right_b =
        corner_point(contours[best_index], rightest_index, 90, -1);
    cv::Point bottom_a =
        corner_point(contours[best_index], bottomest_index, 180, -1);
    cv::Point bottom_b =
        corner_point(contours[best_index], bottomest_index, 0, 1);
    cv::Point left_bottom =
        inter(left_a, left_a.x - left_b.x, left_a.y - left_b.y, bottom_a,
              bottom_a.x - bottom_b.x, bottom_a.y - bottom_b.y);
    cv::Point right_bottom =
        inter(right_a, right_a.x - right_b.x, right_a.y - right_b.y, bottom_a,
              bottom_a.x - bottom_b.x, bottom_a.y - bottom_b.y);
    int inter_ax = (left_bottom.x + right_bottom.x) / 2;
    console->info("inter_ax == {}", inter_ax);
    double center = kFrameWidth / 2 - inter_ax;
    console->info("Dist To center == {}", center);
    double line_length = sqrt(pow(left_bottom.x - right_bottom.x, 2) +
                              pow(left_bottom.y - right_bottom.y, 2));
    console->info("Line Length == {}", line_length);
    double pixel_to_thing = 0.085;
    double dist_inches = (double)(20.0 / 2.0) /
                         (tan(line_length / 2 * pixel_to_thing * M_PI / 180));
    console->info("dist_inches  == {}", dist_inches);
    console->info("pixel_to_thing = {}", pixel_to_thing);
    payload[0] = center;
    payload[1] = dist_inches;
    payload[2] = 0;
    message->send(payload);
  }
}

int main(int argc, char** argv) {
  auto console = spd::stdout_logger_st("console", true);
  std::shared_ptr<deadeye::Config> config;
  try {
    config = std::make_shared<deadeye::Config>(argc, argv);
  } catch (const std::exception& e) {
    console->critical("Config file error: {}", e.what());
    return 1;
  }
  console->info("Deadeye is taking aim...");
  start(config);
}
