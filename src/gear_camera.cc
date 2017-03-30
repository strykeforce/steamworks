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
}

GearCamera::~GearCamera() {
  if (connected_ && has_gui_) {
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

  camera_.reset(new cv::VideoCapture(0));
  if (!camera_->isOpened()) {
    logger_->error("GearCamera::Connect can't open camera");
    return;
  }
  SPDLOG_DEBUG(logger_, "GearCamera::Connect has opened gear camera");

  if (has_gui_) {
    cv::namedWindow("gear");
  }

  connected_ = true;
}

/**
 * Returns true if the camera has been connected.
 */
bool GearCamera::IsConnected() {
  return connected_;
}

/**
 * Process a single frame.
 * Returns azumith error and target height in pixels.
 */
bool GearCamera::ProcessFrame(int& azimuth_error, int& target_height) {
  if (!connected_) {
    logger_->error("GearCamera::ProcessFrame not connected");
    return false;
  }

  *camera_ >> frame_;

  // process frame to find targets and return target data
  if (frame_process_.FindTargets(frame_)) {
    azimuth_error = frame_process_.azimuth_error;
    target_height = frame_process_.target_height;
    return true;
  }

  // no targets found
  return false;
}

void GearCamera::DisplayFrame() {
  if (has_gui_) {
    auto azimuth_offset = frame_process_.GetAzimuthOffset();
    cv::line(frame_, cv::Point(azimuth_offset, 0),
             cv::Point(azimuth_offset, frame_.rows), cv::Scalar(255, 0, 0));

    // draw contours and bounding boxes around targets into captured frame
    if (!frame_process_.right_contour.empty()) {
      cv::drawContours(
          frame_,
          std::vector<std::vector<cv::Point>>{frame_process_.right_contour}, 0,
          cv::Scalar(0, 0, 255), 1);
      cv::rectangle(frame_, frame_process_.right_rect, cv::Scalar(0, 255, 0),
                    1);
    }

    // display capture frame in GUI window

    // cv::imshow("gear", frame_);
    cv::imshow("frame", frame_process_.mask);

    cv::waitKey(1);
  }
}
