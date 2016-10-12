#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"

#include "deadeye_config.h"
#include "deadeye/camera.h"
#include "deadeye/config.h"
#include "deadeye/deadeye.h"
#include "deadeye/robot.h"
#include "deadeye/target.h"

namespace spd = spdlog;

void start(std::shared_ptr<deadeye::Config> config) {
  auto console = spd::get("console");

  std::unique_ptr<deadeye::Robot> robot(new deadeye::Robot(config));
  std::unique_ptr<deadeye::Camera> camera(new deadeye::Camera(config));
  std::unique_ptr<deadeye::Deadeye> deadeye(new deadeye::Deadeye(config));

  for (;;) {
    cv::Mat frame;
    // TODO: check result of Read
    camera->Read(frame);
    auto target_contour = deadeye->TargetContour(frame);

    if (target_contour.empty()) {
      robot->NoTarget();
      continue;
    }

    // #define points target_contour
    cv::Point leftest = (target_contour)[0], rightest = leftest,
              bottomest = leftest, topest = leftest;
    int leftest_index = 0, rightest_index = leftest_index,
        bottomest_index = leftest_index;

    // find left, top, right and bottom-most contour points
    for (uint i = 1; i < target_contour.size(); i++) {
      if ((target_contour)[i].x < leftest.x) {
        leftest = (target_contour)[i];
        leftest_index = i;
      }
      if ((target_contour)[i].x > rightest.x) {
        rightest = (target_contour)[i];
        rightest_index = i;
      }
      if ((target_contour)[i].y > bottomest.y) {
        bottomest = (target_contour)[i];
        bottomest_index = i;
      }
      if ((target_contour)[i].y < topest.y) {
        topest = (target_contour)[i];
      }
    }

    cv::Point left_a = corner_point(target_contour, leftest_index, 90, 1);
    cv::Point left_b = corner_point(target_contour, leftest_index, 270, -1);
    cv::Point right_a = corner_point(target_contour, rightest_index, 270, 1);
    cv::Point right_b = corner_point(target_contour, rightest_index, 90, -1);
    cv::Point bottom_a = corner_point(target_contour, bottomest_index, 180, -1);
    cv::Point bottom_b = corner_point(target_contour, bottomest_index, 0, 1);
    cv::Point left_bottom =
        inter(left_a, left_a.x - left_b.x, left_a.y - left_b.y, bottom_a,
              bottom_a.x - bottom_b.x, bottom_a.y - bottom_b.y);
    cv::Point right_bottom =
        inter(right_a, right_a.x - right_b.x, right_a.y - right_b.y, bottom_a,
              bottom_a.x - bottom_b.x, bottom_a.y - bottom_b.y);
    int inter_ax = (left_bottom.x + right_bottom.x) / 2;
    console->debug("inter_ax == {}", inter_ax);
    double center = camera->framewidth / 2 - inter_ax;
    console->debug("Dist To center == {}", center);
    double line_length = sqrt(pow(left_bottom.x - right_bottom.x, 2) +
                              pow(left_bottom.y - right_bottom.y, 2));
    console->debug("Line Length == {}", line_length);
    double pixel_to_thing = 0.085;
    double dist_inches = (double)(20.0 / 2.0) /
                         (tan(line_length / 2 * pixel_to_thing * M_PI / 180));
    console->debug("dist_inches  == {}", dist_inches);
    console->debug("pixel_to_thing = {}", pixel_to_thing);
    robot->TargetAt(center, dist_inches);
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
  console->info("Deadeye version {} is taking aim...", DEADEYE_VERSION);
  start(config);
}
