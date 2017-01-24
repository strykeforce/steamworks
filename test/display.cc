#include "opencv2/opencv.hpp"
#include <string>
#include "cpptoml/cpptoml.h"

#include "frame.h"

using namespace std;

int main(int argc, char const* argv[]) {
  deadeye::Frame frame{std::shared_ptr<cpptoml::table>{nullptr}};
  string img_path(argv[1]);
  auto image = cv::imread(img_path);
  frame.Process(image);
  cv::Rect lower_rect = cv::boundingRect(frame.lower_contour);
  cv::Rect upper_rect = cv::boundingRect(frame.upper_contour);

  cv::drawContours(image,
                   std::vector<std::vector<cv::Point>>{frame.lower_contour}, 0,
                   cv::Scalar(0, 0, 255), 1);
  cv::drawContours(image,
                   std::vector<std::vector<cv::Point>>{frame.upper_contour}, 0,
                   cv::Scalar(0, 0, 255), 1);

  cv::rectangle(image, lower_rect, cv::Scalar(0, 255, 0), 1);
  cv::rectangle(image, upper_rect, cv::Scalar(0, 255, 0), 1);

  cv::imshow("image", image);

  cv::waitKey(0);

  return 0;
}
