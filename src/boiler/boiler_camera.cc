#include "boiler_camera.h"

#include <cstdlib>

#include "flycapture/FlyCapture2.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

using namespace deadeye;
namespace fc = FlyCapture2;

/** BoilerCamera is the hardware and frame processing routines.
 */
BoilerCamera::BoilerCamera(std::shared_ptr<cpptoml::table> config)
    : logger_(spdlog::get("deadeye")),
      connected_(false),
      capture_started_(false),
      camera_(),
      raw_image_(),
      rgb_image_(),
      width_(1280),
      height_(1080),
      exposure_(-1.0),
      frame_process_(config) {
  LoadConfigSettings(config);
}

BoilerCamera::~BoilerCamera() {
  StopCapture();
  Disconnect();
  if (has_gui_) {
    cv::destroyAllWindows();
  }
}

/** Connect to the hardware.
 */
void BoilerCamera::Connect() {
  if (connected_) {
    return;
  }

  fc::Error error;
  fc::BusManager busMgr;
  unsigned int numCameras;
  error = busMgr.GetNumOfCameras(&numCameras);
  if (error != fc::PGRERROR_OK) {
    logger_->error("BoilerCamera::Connect: busMgr.GetNumOfCameras: {}",
                   error.GetDescription());
    return;
  }

  if (numCameras < 1) {
    logger_->error("BoilerCamera::Connect: busMgr.GetNumOfCameras == 0");
    return;
  }

  fc::PGRGuid guid;
  error = busMgr.GetCameraFromIndex(0, &guid);
  if (error != fc::PGRERROR_OK) {
    logger_->error("BoilerCamera::Connect: busMgr.GetCameraFromIndex: {}",
                   error.GetDescription());
    return;
  }

  // Connect to a camera
  error = camera_.Connect(&guid);
  if (error != fc::PGRERROR_OK) {
    logger_->error("BoilerCamera::Connect: camera.Connect: {}",
                   error.GetDescription());
    return;
  }

  fc::Format7ImageSettings fmt7ImageSettings;
  fmt7ImageSettings.mode = fc::MODE_0;
  fmt7ImageSettings.offsetX = 0;
  fmt7ImageSettings.offsetY = 0;
  fmt7ImageSettings.width = width_;
  fmt7ImageSettings.height = height_;
  fmt7ImageSettings.pixelFormat = fc::PIXEL_FORMAT_RAW8;

  bool valid;
  fc::Format7PacketInfo fmt7PacketInfo;

  // Validate the settings to make sure that they are valid
  error = camera_.ValidateFormat7Settings(&fmt7ImageSettings, &valid,
                                          &fmt7PacketInfo);
  if (error != fc::PGRERROR_OK) {
    logger_->error("BoilerCamera::Connect: camera.ValidateFormat7Settings: {}",
                   error.GetDescription());
    return;
  }

  if (!valid) {
    // Settings are not valid
    logger_->error("BoilerCamera::Connect: Format7 settings are not valid");
    return;
  }

  // Set the settings to the camera
  error = camera_.SetFormat7Configuration(
      &fmt7ImageSettings, fmt7PacketInfo.recommendedBytesPerPacket);
  if (error != fc::PGRERROR_OK) {
    logger_->error("BoilerCamera::Connect: camera.SetFormat7Configuration: {}",
                   error.GetDescription());
    return;
  }

  fc::Property prop;
  prop.type = fc::AUTO_EXPOSURE;
  prop.onOff = true;            // on to control
  prop.autoManualMode = false;  // off for full manual control
  prop.absControl = true;
  prop.absValue = exposure_;
  error = camera_.SetProperty(&prop);
  if (error != fc::PGRERROR_OK) {
    logger_->error("BoilerCamera::Connect: camera.SetProperty: {}",
                   error.GetDescription());
  }
  connected_ = true;
}

/**
 * Disconnect a connected camera.
 */
void BoilerCamera::Disconnect() {
  if (!connected_) {
    return;
  }
  camera_.Disconnect();
  connected_ = false;
}

/**
 * Returns true if the camera has been connected.
 */
bool BoilerCamera::IsConnected() {
  return connected_;
}

/**
 * Start camera capturing
 */
void BoilerCamera::StartCapture() {
  if (!connected_) {
    logger_->error("must call Connect before calling StartCapture");
    return;
  }
  if (capture_started_) {
    logger_->warn("StartCapture already called");
    return;
  }
  SPDLOG_TRACE(logger_, "calling camera_.StartCapture()");
  fc::Error error = camera_.StartCapture();
  SPDLOG_TRACE(logger_, "done calling camera_.StartCapture()");
  if (error != fc::PGRERROR_OK) {
    logger_->error("BoilerCamera::StartCapture: camera.StartCapture: {}",
                   error.GetDescription());
  }
  if (has_gui_) {
    cv::namedWindow("boiler");
  }
  capture_started_ = true;
}

/** Stop camera capturing
 */
void BoilerCamera::StopCapture() {
  if (!connected_ && !capture_started_) {
    return;
  }
  fc::Error error = camera_.StopCapture();
  if (error != fc::PGRERROR_OK) {
    logger_->error("BoilerCamera::StopCapture: camera.StopCapture: {}",
                   error.GetDescription());
  }
  capture_started_ = false;
}

/** Process a single frame.
 * Returns azumith error and target top-edge distance in pixels.
 */
bool BoilerCamera::ProcessFrame(int& azimuth_error, int& centerline_error) {
  if (!connected_ && !capture_started_) {
    logger_->error("not connected or capture not started");
    return false;
  }

  fc::Error error = camera_.RetrieveBuffer(&raw_image_);
  if (error != fc::PGRERROR_OK) {
    logger_->error("BoilerCamera::ProcessFrame: camera.RetrieveBuffer: {}",
                   error.GetDescription());
    return false;
  }

  // convert to RGB
  raw_image_.Convert(fc::PIXEL_FORMAT_BGR, &rgb_image_);

  // convert to OpenCV Mat
  unsigned int row_bytes =
      static_cast<double>(rgb_image_.GetReceivedDataSize()) /
      static_cast<double>(rgb_image_.GetRows());
  frame_ = cv::Mat(rgb_image_.GetRows(), rgb_image_.GetCols(), CV_8UC3,
                   rgb_image_.GetData(), row_bytes);

  // process frame to find targets and return target data
  if (frame_process_.FindTargets(frame_)) {
    azimuth_error = frame_process_.azimuth_error;
    centerline_error = frame_process_.centerline_error;
    return true;
  }

  // no targets found
  return false;
}

void BoilerCamera::DisplayFrame() {
  if (!has_gui_) {
    return;
  }
  // draw contours and bounding boxes around targets into captured frame
  cv::drawContours(
      frame_, std::vector<std::vector<cv::Point>>{frame_process_.lower_contour},
      0, cv::Scalar(0, 0, 255), 1);
  cv::drawContours(
      frame_, std::vector<std::vector<cv::Point>>{frame_process_.upper_contour},
      0, cv::Scalar(0, 0, 255), 1);

  cv::rectangle(frame_, frame_process_.lower_rect, cv::Scalar(0, 255, 0), 1);
  cv::rectangle(frame_, frame_process_.upper_rect, cv::Scalar(0, 255, 0), 1);

  cv::line(frame_, cv::Point((frame_.cols / 2), 0),
           cv::Point((frame_.cols / 2), frame_.rows), cv::Scalar(255, 0, 0));

  cv::line(frame_, cv::Point(0, frame_.rows / 2),
           cv::Point(frame_.cols, frame_.rows / 2), cv::Scalar(255, 0, 0));

  std::string centerline_error =
      "centerline error (px) = " +
      std::to_string(frame_process_.centerline_error);
  std::string coords =
      "azimuth error (px )= " + std::to_string(frame_process_.azimuth_error);

  // TODO: add upper rect width
  // std::string coords =
  //     "azimuth error (px )= " +
  //     std::to_string(frame_process_.azimuth_error);

  cv::putText(frame_, centerline_error, cv::Point(100, 50),
              cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255));
  cv::putText(frame_, coords, cv::Point(100, 100), cv::FONT_HERSHEY_SIMPLEX,
              1.0, cv::Scalar(255, 255, 255));

  // display capture frame in GUI window
  cv::imshow("boiler", frame_);
  // cv::imshow("boiler", frame_process_.mask);
  // }
  cv::waitKey(1);
}

/**
 * Load configuration.
 */
void BoilerCamera::LoadConfigSettings(
    const std::shared_ptr<cpptoml::table> config_in) {
  assert(config_in);
  auto config = config_in->get_table("BOILER");

  bool has_display = !!std::getenv("DISPLAY");
  auto b_opt = config->get_as<bool>("gui");
  if (b_opt) {
    has_gui_ = *b_opt && has_display;
  }
  logger_->info("has display = {}, gui = {}", has_display, has_gui_);

  config = config->get_table("CAMERA");
  if (config) {
    auto width = config->get_as<int>("width");
    if (width) {
      width_ = *width;
    } else {
      logger_->warn("CAMERA width setting missing, using default");
    }

    auto height = config->get_as<int>("height");
    if (height) {
      height_ = *height;
    } else {
      logger_->warn("CAMERA height setting missing, using default");
    }

    auto exposure = config->get_as<double>("exposure");
    if (exposure) {
      exposure_ = *exposure;
    } else {
      logger_->warn("CAMERA exposure setting missing, using default");
    }
  } else {
    logger_->error("CAMERA configuration section missing, using defaults");
  }
  logger_->info("camera frame width: {}", width_);
  logger_->info("camera frame height: {}", height_);
  logger_->info("camera frame exposure: {}", exposure_);
}
