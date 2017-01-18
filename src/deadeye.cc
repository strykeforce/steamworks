#include "deadeye.h"

#include "FlyCapture2.h"
#include <memory>
#include <iostream>
#include "opencv2/opencv.hpp"

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

    cv::Mat hsl, blur, mask;
    cv::cvtColor(image, hsl, CV_BGR2HSV);
    cv::blur(hsl, blur, cv::Size(2, 2));
    cv::inRange(blur, cv::Scalar{80, 100, 100}, cv::Scalar{100, 255, 255},
                mask);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask.clone(), contours, CV_RETR_LIST,
                     CV_CHAIN_APPROX_SIMPLE);

    std::vector<cv::Point> lower, upper;
    double arc = 0.0;
    for (const auto& c : contours) {
      auto candidate = cv::arcLength(c, true);
      if (candidate > arc) {
        lower = upper;
        upper = c;
        arc = candidate;
      }
    }

    cv::Rect lower_rect = cv::boundingRect(lower);
    cv::Rect upper_rect = cv::boundingRect(upper);

    cv::drawContours(image, std::vector<std::vector<cv::Point>>{lower}, 0,
                     cv::Scalar(0, 0, 255), 1);
    cv::drawContours(image, std::vector<std::vector<cv::Point>>{upper}, 0,
                     cv::Scalar(0, 0, 255), 1);

    cv::rectangle(image, lower_rect, cv::Scalar(0, 255, 0), 1);
    cv::rectangle(image, upper_rect, cv::Scalar(0, 255, 0), 1);

    cv::imshow("image", image);

    key = cv::waitKey(30);
  }

  error = camera_->StopCapture();
  if (error != PGRERROR_OK) {
    // This may fail when the camera was removed, so don't show
    // an error message
  }

  camera_->Disconnect();
}
