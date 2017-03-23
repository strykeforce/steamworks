#include "boiler_frame.h"

using namespace deadeye;

BoilerFrame::BoilerFrame(std::shared_ptr<cpptoml::table> config)
    : logger_(spdlog::get("deadeye")),
      hsv_lower_(80, 100, 100),
      hsv_upper_(100, 255, 255),
      min_arc_length_(250.0) {
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

  double arc_length = 0.0;
  for (const auto& c : contours) {
    auto candidate = cv::arcLength(c, true);
    if (candidate > arc_length) {
      lower_contour = upper_contour;
      upper_contour = c;
      arc_length = candidate;
    }
  }

  if (arc_length < min_arc_length_) {
    // failed to find a target
    return false;
  }

  // using upright bounding box to find top edges of targets
  lower_rect = cv::boundingRect(lower_contour);
  upper_rect = cv::boundingRect(upper_contour);
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

    auto min_arc = config->get_as<double>("min_arc_length");
    if (min_arc) {
      min_arc_length_ = *min_arc;
    } else {
      logger_->warn(
          "BOILER.FRAME min_arc_length setting missing, using default");
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
