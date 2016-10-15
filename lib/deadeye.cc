#include "deadeye/deadeye.h"

#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"

#include "deadeye/camera.h"
#include "deadeye/config.h"
#include "deadeye/robot.h"
#include "deadeye/target.h"

namespace spd = spdlog;

namespace deadeye {

Deadeye::Deadeye(std::shared_ptr<deadeye::Config> config) {
  auto console = spd::get("console");
  auto target = config->GetTable("target");

  // TODO: validate returned parameters
  auto blur_size = target->get_array_of<int64_t>("blur_size");
  blur_size_ = cv::Size{(int)(*blur_size)[0], (int)(*blur_size)[1]};

  auto hsv_l = target->get_array_of<int64_t>("hsv_lower");
  auto hsv_u = target->get_array_of<int64_t>("hsv_upper");

  lower_ = cv::Scalar((*hsv_l)[0], (*hsv_l)[1], (*hsv_l)[2]);
  upper_ = cv::Scalar((*hsv_u)[0], (*hsv_u)[1], (*hsv_u)[2]);

  min_perimeter_ = *target->get_as<int>("min_perimeter");
  is_closing_ = *target->get_as<bool>("closing");

  console->info("frame: range_lower = ({}, {}, {})", lower_[0], lower_[1],
                lower_[2]);
  console->info("frame: range_upper = ({}, {}, {})", upper_[0], upper_[1],
                upper_[2]);
  console->info("frame: blur kernel size (w, h) = ({}, {})", blur_size_.width,
                blur_size_.height);
  console->info("frame: performing morphological closing: {}", is_closing_);
  console->info("contour: perimeter threshold = {}", min_perimeter_);

  robot_ = std::unique_ptr<deadeye::Robot>(new deadeye::Robot(config));
  camera_ = std::unique_ptr<deadeye::Camera>(new deadeye::Camera(config));
}

Deadeye::~Deadeye() {
  spd::get("console")->debug("Releasing camera.");
  camera_.release();
}

void Deadeye::SetFrameCallback(ImageCallbackFunc func) {
  frameCallback_ = func;
}

void Deadeye::SetMaskCallback(ImageCallbackFunc func) {
  maskCallback_ = func;
}

void Deadeye::Start() {
  auto console = spd::get("console");
  for (;;) {
    cv::Mat frame;
    // TODO: check result of Read
    camera_->Read(frame);

    if (frameCallback_) {
      if (frameCallback_(frame)) {  // returns true to quit
        break;
      }
    }

    auto target_contour = TargetContour(frame);

    if (maskCallback_) {
      if (maskCallback_(mask_)) {  // returns true to quit
        break;
      }
    }

    if (target_contour.empty()) {
      robot_->NoTarget();
      continue;
    }

    auto corners = FindBottomCorners(target_contour);
    auto aim = FindAimPoint(std::move(corners));
    robot_->TargetAt(aim->center, aim->dist_inches);
  }
}

std::vector<cv::Point> Deadeye::TargetContour(const cv::Mat& frame) {
  auto console = spd::get("console");

  // TODO: get rid of morph. closing
  cv::cvtColor(frame, frame, CV_BGR2HSV);
  cv::blur(frame, frame, blur_size_);
  cv::inRange(frame, lower_, upper_, mask_);
  if (is_closing_) {
    cv::dilate(mask_, mask_, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
    cv::erode(mask_, mask_, cv::Mat(), cv::Point(-1, -1), 1, 1, 1);
  }
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(mask_.clone(), contours, CV_RETR_LIST,
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

  if (cv::arcLength(contours[best_index], true) < min_perimeter_) {
    return {};
  }

  return contours[best_index];
}

std::unique_ptr<BottomCorners> Deadeye::FindBottomCorners(
    const std::vector<cv::Point>& target_contour) {
  // #define points target_contour
  cv::Point leftest = target_contour[0], rightest = leftest,
            bottomest = leftest, topest = leftest;
  int leftest_index = 0, rightest_index = leftest_index,
      bottomest_index = leftest_index;

  // find left, top, right and bottom-most contour points
  for (uint i = 1; i < target_contour.size(); i++) {
    if (target_contour[i].x < leftest.x) {
      leftest = target_contour[i];
      leftest_index = i;
    }
    if (target_contour[i].x > rightest.x) {
      rightest = target_contour[i];
      rightest_index = i;
    }
    if (target_contour[i].y > bottomest.y) {
      bottomest = target_contour[i];
      bottomest_index = i;
    }
    if (target_contour[i].y < topest.y) {
      topest = target_contour[i];
    }
  }

  cv::Point left_a = corner_point(target_contour, leftest_index, 90, 1);
  cv::Point left_b = corner_point(target_contour, leftest_index, 270, -1);
  cv::Point right_a = corner_point(target_contour, rightest_index, 270, 1);
  cv::Point right_b = corner_point(target_contour, rightest_index, 90, -1);
  cv::Point bottom_a = corner_point(target_contour, bottomest_index, 180, -1);
  cv::Point bottom_b = corner_point(target_contour, bottomest_index, 0, 1);

  std::unique_ptr<BottomCorners> result(new BottomCorners());
  result->left =
      inter(left_a, left_a.x - left_b.x, left_a.y - left_b.y, bottom_a,
            bottom_a.x - bottom_b.x, bottom_a.y - bottom_b.y);
  result->right =
      inter(right_a, right_a.x - right_b.x, right_a.y - right_b.y, bottom_a,
            bottom_a.x - bottom_b.x, bottom_a.y - bottom_b.y);
  return result;
}

std::unique_ptr<AimPoint> Deadeye::FindAimPoint(
    std::unique_ptr<BottomCorners> corners) {
  int inter_ax = (corners->left.x + corners->right.x) / 2;

  std::unique_ptr<AimPoint> result(new AimPoint());
  result->center = camera_->framewidth / 2 - inter_ax;
  double line_length = sqrt(pow(corners->left.x - corners->right.x, 2) +
                            pow(corners->left.y - corners->right.y, 2));
  double pixel_to_thing = 0.085;
  result->dist_inches = (double)(20.0 / 2.0) /
                        (tan(line_length / 2 * pixel_to_thing * M_PI / 180));
  return result;
}

} /* deadeye */
