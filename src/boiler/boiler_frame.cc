#include "boiler_frame.h"

using namespace deadeye;
using namespace std;

BoilerFrame::BoilerFrame(std::shared_ptr<cpptoml::table> config)
    : logger_(spdlog::get("deadeye")),
      hsv_lower_(80, 100, 100),
      hsv_upper_(100, 255, 255) {
  LoadConfigSettings(config);
}

/** Process the frame and find targets.
 */
bool BoilerFrame::FindTargets(const cv::Mat& frame) {
  cv::cvtColor(frame, hsv, CV_BGR2HSV);
  // full frame blur drops from 30 fps to 13 fps
  // cv::blur(hsv, blur, cv::Size(2, 2));
  cv::inRange(hsv, hsv_lower_, hsv_upper_, mask);

  std::vector<std::vector<cv::Point>> contours;
  // TODO: see if we can eliminate this clone
  cv::findContours(mask.clone(), contours, CV_RETR_LIST,
                   CV_CHAIN_APPROX_SIMPLE);

  sort(contours.begin(), contours.end(),
       [](vector<cv::Point> a, vector<cv::Point> b) {
         return cv::arcLength(b, true) < cv::arcLength(a, true);
       });

  vector<int> target_idx;
  // vector<cv::Rect> target_rects;

  int found = 0;
  for (size_t i = 0; i < contours.size(); i++) {
    if (found == 2)
      break;

    auto arc = cv::arcLength(contours[i], true);
    if (arc < min_arc_length_) {
      // SPDLOG_DEBUG(logger_, "boiler arc length fail: {}", arc);
      continue;
    }

    auto rect = cv::boundingRect(contours[i]);
    double aspect_ratio = static_cast<double>(rect.width) / rect.height;
    if (aspect_ratio < aspect_ratio_min_ || aspect_ratio > aspect_ratio_max_) {
      SPDLOG_DEBUG(logger_, "boiler aspect ratio fail: {}", aspect_ratio);
      continue;
    }

    target_idx.push_back(i);
    // target_rects.push_back(rect);
    found++;
  }

  auto targets = target_idx.size();
  // SPDLOG_DEBUG(logger_, "boiler targets found = {}", targets);

  switch (targets) {
    case 0:
      return false;
    case 1:
      upper_contour = contours[target_idx[0]];
      upper_rect = cv::boundingRect(upper_contour);
      // SPDLOG_DEBUG(logger_, "one target detected upper y = {}, lower y = {}",
      //              upper_rect.y, lower_rect.y);
      return false;
    case 2:
      upper_contour = contours[target_idx[0]];
      lower_contour = contours[target_idx[1]];
      upper_rect = cv::boundingRect(upper_contour);
      lower_rect = cv::boundingRect(lower_contour);
      // SPDLOG_DEBUG(logger_, "two targets detected upper y = {}, lower y =
      // {}",
      //              upper_rect.y, lower_rect.y);
      break;
    default:
      logger_->error("boiler frame switch should not reach default");
  }

  // logger_->info("lower rect width = {}", lower_rect.width);

  // compute distance between target bounding box top edges and send to robot
  int centerline = (lower_rect.y + upper_rect.y) / 2;
  centerline_error = centerline - (frame.rows / 2);
  azimuth_error = (frame.cols / 2) - upper_rect.x - (upper_rect.width / 2);

  return true;
}

/**
 * Load configuration
 */
void BoilerFrame::LoadConfigSettings(
    const std::shared_ptr<cpptoml::table> config_in) {
  assert(config_in);
  auto config = config_in->get_table("BOILER")->get_table("FRAME");
  if (!config) {
    throw std::invalid_argument("BOILER.FRAME table missing from config");
  }

  if (config) {
    auto hsv_l = config->get_array_of<int64_t>("hsv_lower");
    if (hsv_l) {
      hsv_lower_ = cv::Scalar((*hsv_l)[0], (*hsv_l)[1], (*hsv_l)[2]);
    } else {
      logger_->warn("BOILER.FRAME hsv_lower setting missing, using default");
    }

    auto hsv_u = config->get_array_of<int64_t>("hsv_upper");
    if (hsv_u) {
      hsv_upper_ = cv::Scalar((*hsv_u)[0], (*hsv_u)[1], (*hsv_u)[2]);
    } else {
      logger_->warn("BOILER.FRAME hsv_upper setting missing, using default");
    }

    auto d_opt = config->get_as<double>("min_arc_length");
    if (d_opt) {
      min_arc_length_ = *d_opt;
    } else {
      logger_->warn("BOILER.FRAME min_arc_length setting missing, using {}",
                    min_arc_length_);
    }

    d_opt = config->get_as<double>("aspect_ratio_min");
    if (d_opt) {
      aspect_ratio_min_ = *d_opt;
    } else {
      logger_->warn("BOILER.FRAME aspect_ratio_min setting missing, using {}",
                    aspect_ratio_min_);
    }

    d_opt = config->get_as<double>("aspect_ratio_max");
    if (d_opt) {
      aspect_ratio_max_ = *d_opt;
    } else {
      logger_->warn("BOILER.FRAME aspect_ratio_max setting missing, using {}",
                    aspect_ratio_max_);
    }

  } else {
    logger_->error(
        "BOILER.FRAME configuration section missing, using defaults");
  }

  logger_->info("BOILER HSV lower: {}, {}, {}", hsv_lower_[0], hsv_lower_[1],
                hsv_lower_[2]);
  logger_->info("BOILER HSV upper: {}, {}, {}", hsv_upper_[0], hsv_upper_[1],
                hsv_upper_[2]);
  logger_->info("BOILER min arc Length: {}", min_arc_length_);
}
