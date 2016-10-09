#include "camera.h"

#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"

#include "config.h"
#include "constants.h"

namespace spd = spdlog;

namespace deadeye {
Camera::Camera(std::shared_ptr<deadeye::Config> config) {
  framewidth = kFrameWidth;
  ConfigCameraV4L2();

  camera_ = std::unique_ptr<cv::VideoCapture>(new cv::VideoCapture(0));
  camera_->set(CV_CAP_PROP_FRAME_WIDTH, kFrameWidth);
  camera_->set(CV_CAP_PROP_FRAME_HEIGHT, kFrameHeight);
  camera_->set(CV_CAP_PROP_BRIGHTNESS, 0.0);
}
Camera::~Camera() {}

bool Camera::Read(cv::Mat& frame) {
  return camera_->read(frame);
}

void Camera::ConfigCameraV4L2() {
  auto console = spd::get("console");
  console->debug("NVIDIAUtils::ConfigCameraV4L2()");
  int status1, status2, status3, status4;
  do {
    status1 = system("uvcdynctrl -s Exposure,\\ Auto 1");
    console->debug("status2 == {}", status1);
    status2 = system("uvcdynctrl -s Exposure\\ \\(Absolute\\) 10");
    console->debug("status1 == {}", status2);
    status3 = system("uvcdynctrl -s White\\ Balance\\ Temperature,\\ Auto 0");
    console->debug("status4 == {}", status3);
    status4 = system("uvcdynctrl -s White\\ Balance\\ Temperature 4500");
    console->debug("status3 == {}", status4);
    usleep(1 * 1000 * 1000);
  } while (status1 | status2 | status3 | status4);
}

} /* deadeye */
