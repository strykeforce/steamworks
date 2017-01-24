#include "deadeye.h"

#include "FlyCapture2.h"
#include <memory>
#include <iostream>
#include <sstream>
#include "opencv2/opencv.hpp"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "frame.h"
#include "robot.h"

using namespace FlyCapture2;
using namespace deadeye;

Deadeye::Deadeye(std::shared_ptr<FlyCapture2::Camera> camera)
    : camera_(camera) {}

void Deadeye::Display() {
  auto logger = spdlog::get("deadeye");
  logger->set_level(spdlog::level::debug);

  Error error = camera_->StartCapture();
  if (error == PGRERROR_ISOCH_BANDWIDTH_EXCEEDED) {
    logger->error("bandwidth exceeded");
  } else if (error != PGRERROR_OK) {
    logger->error("failed to start image capture");
  }

  // capture loop
  Frame frame{std::shared_ptr<cpptoml::table>{nullptr}};
  Robot robot{std::shared_ptr<cpptoml::table>{nullptr}};
  char key = 0;
  while (key != 'q') {
    // Get the image
    Image rawImage;
    Error error = camera_->RetrieveBuffer(&rawImage);
    if (error != PGRERROR_OK) {
      logger->error("capture error");
      continue;
    }

    // convert to RGB
    Image rgbImage;
    rawImage.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage);

    // convert to OpenCV Mat
    unsigned int rowBytes =
        (double)rgbImage.GetReceivedDataSize() / (double)rgbImage.GetRows();
    cv::Mat image = cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3,
                            rgbImage.GetData(), rowBytes);

    // process image in OpenCV and find target contours
    if (!frame.FindTargets(image)) {
      robot.SendBoilerNoTarget();
    };

    // using upright bounding box to find top edges of targets
    cv::Rect lower_rect = cv::boundingRect(frame.lower_contour);
    cv::Rect upper_rect = cv::boundingRect(frame.upper_contour);

    // draw contours and bounding boxes around targets into captured frame
    cv::drawContours(image,
                     std::vector<std::vector<cv::Point>>{frame.lower_contour},
                     0, cv::Scalar(0, 0, 255), 1);
    cv::drawContours(image,
                     std::vector<std::vector<cv::Point>>{frame.upper_contour},
                     0, cv::Scalar(0, 0, 255), 1);

    cv::rectangle(image, lower_rect, cv::Scalar(0, 255, 0), 1);
    cv::rectangle(image, upper_rect, cv::Scalar(0, 255, 0), 1);

    // compute distance between target bounding box top edges and send to robot
    auto y = lower_rect.y - upper_rect.y;
    logger->debug("Y distance = {}", y);
    robot.SendBoilerTarget(0, y);

    // display capture frame in GUI window
    // cv::imshow("image", image);

    key = cv::waitKey(30);
  }

  error = camera_->StopCapture();
  if (error != PGRERROR_OK) {
    logger->error("camera error in StopCapture()");
  }

  camera_->Disconnect();
}
