#include "gear_camera.h"

#include <cstdlib>
#include <stdlib.h>

using namespace deadeye;
using namespace std;

/** GearCamera is the hardware and frame processing routines.
 */
GearCamera::GearCamera(std::shared_ptr<cpptoml::table> config_in)
    : logger_(spdlog::get("deadeye")),
      connected_(false),
      capture_started_(false),
      has_gui_(!!std::getenv("DISPLAY")),
      exposure_(-1.0),
      frame_process_(config_in) {
  assert(config_in);
  auto config = config_in->get_table("GEAR");
  if (!config) {
    throw invalid_argument("GEAR config table missing");
  }

  config = config->get_table("CAMERA");
  if (!config) {
    throw invalid_argument("GEAR.CAMERA config table missing");
  }

  auto d_opt = config->get_as<double>("exposure");
  if (d_opt) {
    exposure_ = *d_opt;
  } else {
    logger_->warn("CAMERA exposure setting missing, using default");
  }
  logger_->info("camera frame exposure: {}", exposure_);
}

GearCamera::~GearCamera() {
  StopCapture();
  Disconnect();
  if (has_gui_) {
    cv::destroyAllWindows();
  }
}

/**
 * Configure the camera.
 */
void GearCamera::Connect() {
  if (connected_) {
    return;
  }
  auto ret = system("/usr/bin/uvcdynctrl --load=/etc/deadeye/lifecam.conf");
  if (ret != 0) {
    logger_->error("error setting LifeCam exposure with uvcdynctrl");
  }
  connected_ = true;
}

/**
 * Disconnect the camera.
 */
void GearCamera::Disconnect() {
  if (!connected_) {
    return;
  }
  connected_ = false;
}

/**
 * Returns true if the camera has been connected.
 */
bool GearCamera::IsConnected() {
  return connected_;
}

/**
 * Start camera capturing
 */
void GearCamera::StartCapture() {
  if (!connected_) {
    logger_->error("must call Connect before calling StartCapture");
    return;
  }
  if (capture_started_) {
    logger_->warn("StartCapture already called");
    return;
  }
  if (has_gui_) {
    cv::namedWindow("frame");
  }

  camera_.reset(new cv::VideoCapture(0));
  if (!camera_->isOpened()) {
    logger_->error("StartCapture can't open camera");
    return;
  }
  logger_->debug("StartCapture has opened gear camera");

  capture_started_ = true;
}

/**
 * Stop camera capturing
 */
void GearCamera::StopCapture() {
  if (!connected_ && !capture_started_) {
    return;
  }
  camera_.reset(nullptr);
  capture_started_ = false;
}

const std::tuple<int, int> GearCamera::PROCESS_ERROR = std::make_tuple(-1, -1);

/**
 * Process a single frame.
 * Returns azumith error and target top-edge distance in pixels.
 */
std::tuple<int, int> GearCamera::ProcessFrame() {
  if (!connected_ && !capture_started_) {
    logger_->error("not connected or capture not started");
    return PROCESS_ERROR;
  }

  *camera_ >> frame_;

  // no targets found
  return PROCESS_ERROR;
}

void GearCamera::DisplayFrame() {
  if (has_gui_) {
    // draw contours and bounding boxes around targets into captured frame
    // cv::drawContours(
    //     frame_,
    //     std::vector<std::vector<cv::Point>>{frame_process_.left_contour}, 0,
    //     cv::Scalar(0, 0, 255), 1);
    // cv::drawContours(
    //     frame_,
    //     std::vector<std::vector<cv::Point>>{frame_process_.right_contour}, 0,
    //     cv::Scalar(0, 0, 255), 1);
    //
    // cv::rectangle(frame_, frame_process_.left_rect, cv::Scalar(0, 255, 0),
    // 1); cv::rectangle(frame_, frame_process_.right_rect, cv::Scalar(0, 255,
    // 0), 1);
    //
    // cv::line(frame_, cv::Point((frame_.cols / 2), 0),
    //          cv::Point((frame_.cols / 2), frame_.rows), cv::Scalar(255, 0,
    //          0));
    //
    // std::string range = "target separation (px) = " +
    //                     std::to_string(frame_process_.target_separation);
    // std::string coords =
    //     "azimuth error (px )= " +
    //     std::to_string(frame_process_.azimuth_error);
    // cv::putText(frame_, range, cv::Point(100, 50), cv::FONT_HERSHEY_SIMPLEX,
    //             1.0, cv::Scalar(255, 255, 255));
    // cv::putText(frame_, coords, cv::Point(100, 100),
    // cv::FONT_HERSHEY_SIMPLEX,
    //             1.0, cv::Scalar(255, 255, 255));

    // display capture frame in GUI window
    cv::imshow("frame", frame_);
    // if (frame_process_.target_separation < 90 ||
    //     frame_process_.target_separation > 110) {
    //   cv::waitKey(10000);
    //   return;
    // }
    cv::waitKey(1);
  }
}
