#include "gear_camera.h"

#include <cstdlib>
#include <stdlib.h>
#include <sstream>

using namespace deadeye;
using namespace std;

/** GearCamera is the hardware and frame processing routines.
 */
GearCamera::GearCamera(std::shared_ptr<cpptoml::table> config_in)
    : logger_(spdlog::get("deadeye")),
      connected_(false),
      capture_started_(false),
      has_gui_(!!std::getenv("DISPLAY")),
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

  auto i_opt = config->get_as<int>("azimuth_offset");
  if (i_opt) {
    azimuth_offset_ = *i_opt;
  } else {
    logger_->warn("CAMERA azimuth_offset setting missing, using default");
  }
  logger_->info("gear camera azimuth offset: {}", azimuth_offset_);
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
  SPDLOG_DEBUG(logger_, "StartCapture has opened gear camera");

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

/**
 * Process a single frame.
 * Returns azumith error and target top-edge distance in pixels.
 */
bool GearCamera::ProcessFrame(int& azimuth_error, int& target_width) {
  if (!connected_ && !capture_started_) {
    logger_->error("not connected or capture not started");
    return -1;
  }

  *camera_ >> frame_;

  // process frame to find targets and return target data
  if (frame_process_.FindTargets(frame_)) {
    azimuth_error = frame_process_.azimuth_error;
    target_width = 0;
    return true;
  }

  // no targets found
  return false;
}

void GearCamera::DisplayFrame() {
  if (has_gui_) {
    cv::line(frame_, cv::Point(azimuth_offset_, 0),
             cv::Point(azimuth_offset_, frame_.rows), cv::Scalar(255, 0, 0));

    // draw contours and bounding boxes around targets into captured frame
    if (!frame_process_.right_contour.empty()) {
      cv::drawContours(
          frame_,
          std::vector<std::vector<cv::Point>>{frame_process_.right_contour}, 0,
          cv::Scalar(0, 0, 255), 1);
      cv::rectangle(frame_, frame_process_.right_rect, cv::Scalar(0, 255, 0),
                    1);
    }

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
