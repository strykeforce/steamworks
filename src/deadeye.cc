#include "deadeye.h"

#include "FlyCapture2.h"
#include <memory>
#include <iostream>
#include <sstream>
#include "opencv2/opencv.hpp"
#include "cpptoml/cpptoml.h"

#include "frame.h"

using namespace FlyCapture2;
using namespace deadeye;

Deadeye::Deadeye(std::shared_ptr<FlyCapture2::Camera> camera)
    : camera_(camera) {}

void Deadeye::Display() {
  Error error = camera_->StartCapture();
  if (error == PGRERROR_ISOCH_BANDWIDTH_EXCEEDED) {
    std::cout << "Bandwidth exceeded" << std::endl;
  } else if (error != PGRERROR_OK) {
    std::cout << "Failed to start image capture" << std::endl;
  }

  // capture loop
  Frame frame{std::shared_ptr<cpptoml::table>{nullptr}};
  char key = 0;
  while (key != 'q') {
    // Get the image
    Image rawImage;
    Error error = camera_->RetrieveBuffer(&rawImage);
    if (error != PGRERROR_OK) {
      std::cout << "capture error" << std::endl;
      continue;
    }

    // convert to rgb
    Image rgbImage;
    rawImage.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage);

    // convert to OpenCV Mat
    unsigned int rowBytes =
        (double)rgbImage.GetReceivedDataSize() / (double)rgbImage.GetRows();
    cv::Mat image = cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3,
                            rgbImage.GetData(), rowBytes);

    frame.Process(image);
    cv::Rect lower_rect = cv::boundingRect(frame.lower_contour);
    cv::Rect upper_rect = cv::boundingRect(frame.upper_contour);

    cv::drawContours(image,
                     std::vector<std::vector<cv::Point>>{frame.lower_contour},
                     0, cv::Scalar(0, 0, 255), 1);
    cv::drawContours(image,
                     std::vector<std::vector<cv::Point>>{frame.upper_contour},
                     0, cv::Scalar(0, 0, 255), 1);

    cv::rectangle(image, lower_rect, cv::Scalar(0, 255, 0), 1);
    cv::rectangle(image, upper_rect, cv::Scalar(0, 255, 0), 1);

    cv::imshow("image", image);

    key = cv::waitKey(30);
    // auto t = std::time(nullptr);
    // auto tm = *std::localtime(&t);
    // std::ostringstream filename;
    // filename << "boiler_" << std::put_time(&tm, "%Y%m%d%H%M%S") << ".png";
    // cv::imwrite(filename.str(), image);
  }

  error = camera_->StopCapture();
  if (error != PGRERROR_OK) {
    // This may fail when the camera was removed, so don't show
    // an error message
  }

  camera_->Disconnect();
}
