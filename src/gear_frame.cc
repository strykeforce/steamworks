#include "gear_frame.h"

using namespace deadeye;
using namespace std;

GearFrame::GearFrame(shared_ptr<cpptoml::table> config_in)
    : logger_(spdlog::get("deadeye")),
      hsv_lower_(80, 100, 100),
      hsv_upper_(100, 255, 255),
      min_arc_length_(50.0) {
  assert(config_in);
  auto config = config_in->get_table("GEAR");
  if (!config) {
    throw invalid_argument("GEAR config table missing");
  }

  config = config->get_table("FRAME");
  if (!config) {
    throw invalid_argument("GEAR.FRAME config table missing");
  }

  auto hsv_l = config->get_array_of<int64_t>("hsv_lower");
  if (hsv_l) {
    hsv_lower_ = cv::Scalar((*hsv_l)[0], (*hsv_l)[1], (*hsv_l)[2]);
  } else {
    logger_->warn("GEAR.FRAME hsv_lower setting missing, using default");
  }

  auto hsv_u = config->get_array_of<int64_t>("hsv_upper");
  if (hsv_u) {
    hsv_upper_ = cv::Scalar((*hsv_u)[0], (*hsv_u)[1], (*hsv_u)[2]);
  } else {
    logger_->warn("GEAR.FRAME hsv_upper setting missing, using default");
  }

  auto d_opt = config->get_as<double>("min_arc_length");
  if (d_opt) {
    min_arc_length_ = *d_opt;
  } else {
    logger_->warn("GEAR.FRAME min_arc_length setting missing, using default");
  }

  auto i_opt = config->get_as<int>("azimuth_offset");
  if (i_opt) {
    azimuth_offset_ = *i_opt;
  } else {
    logger_->warn("CAMERA azimuth_offset setting missing, using default");
  }
  logger_->info("gear camera azimuth offset: {}", azimuth_offset_);

  logger_->info("GEAR HSV lower: {}, {}, {}", hsv_lower_[0], hsv_lower_[1],
                hsv_lower_[2]);
  logger_->info("GEAR HSV upper: {}, {}, {}", hsv_upper_[0], hsv_upper_[1],
                hsv_upper_[2]);
  logger_->info("GEAR min arc Length: {}", min_arc_length_);
}

/**
 * Azimuth offset is used by gear camera.
 */
int GearFrame::GetAzimuthOffset() {
  return azimuth_offset_;
}

/** Process the frame and find targets.
 */
bool GearFrame::FindTargets(const cv::Mat& frame) {
  cv::cvtColor(frame, hsv, CV_BGR2HSV);
  cv::inRange(hsv, hsv_lower_, hsv_upper_, mask);
  // cv::blur(mask, mask, cv::Size(2, 2));

  left_contour.clear();
  right_contour.clear();
  vector<vector<cv::Point>> contours;
  // TODO: see if we can eliminate this clone
  cv::findContours(mask.clone(), contours, CV_RETR_LIST,
                   CV_CHAIN_APPROX_SIMPLE);

  SPDLOG_DEBUG(logger_, "GearFrame found {} contours", contours.size());
  sort(contours.begin(), contours.end(),
       [](vector<cv::Point> a, vector<cv::Point> b) {
         return cv::arcLength(b, true) < cv::arcLength(a, true);
       });

  vector<int> target_idx;
  vector<cv::Rect> target_rects;

  int found = 0;
  for (size_t i = 0; i < contours.size(); i++) {
    if (found == 2)
      break;

    auto rect = cv::boundingRect(contours[i]);
    double aspect_ratio = static_cast<double>(rect.width) / rect.height;
    double error = fabs((2.0 / 5) - aspect_ratio);
    if (error > 0.05) {
      continue;
    }
    SPDLOG_DEBUG(logger_, "GearFrame contour {} has aspect {:f} with area {}",
                 found, aspect_ratio, contourArea(contours[i]));
    auto area = contourArea(contours[i]);
    if (area > 300) {
      target_idx.push_back(i);
      target_rects.push_back(rect);
      found++;
    }
  }

  auto targets = target_idx.size();

  // has_valid_target = true;
  switch (targets) {
    case 0:
      return false;
    case 1:
      has_left_target = false;
      right_contour = contours[target_idx[0]];
      right_rect = target_rects[target_idx[0]];
      break;
    case 2:
      has_left_target = true;
      if (target_rects[0].x > target_rects[1].x) {
        right_contour = contours[target_idx[0]];
        right_rect = target_rects[target_idx[0]];
        left_contour = contours[target_idx[1]];
        left_rect = target_rects[target_idx[1]];
      } else {
        left_contour = contours[target_idx[0]];
        left_rect = target_rects[target_idx[0]];
        right_contour = contours[target_idx[1]];
        right_rect = target_rects[target_idx[1]];
      }
      break;
  }

  // compute distance between target bounding box top edges and send to robot
  azimuth_error = azimuth_offset_ - right_rect.x - (right_rect.width / 2);
  target_height = right_rect.height;
  target_width = right_rect.width;

  return true;
}
