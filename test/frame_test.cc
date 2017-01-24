#include <vector>

#include "catch/catch.hpp"
#include "opencv2/opencv.hpp"

#include "frame.h"
#include "frame_test_data.h"
using namespace deadeye;

std::string frame_path(std::string frame) {
  std::string path = __FILE__;
  return path.substr(0, path.find_last_of('/') + 1) + frame;
}

TEST_CASE("test contour arc length", "[frame, contour]") {
  Frame frame{std::shared_ptr<cpptoml::table>{nullptr}};
  for (const auto& tc : tests) {
    auto f = cv::imread(frame_path(tc.frame));
    frame.Process(f);
    REQUIRE(cv::arcLength(frame.upper_contour, true) ==
            Approx(tc.upper_arc_length));
    REQUIRE(cv::arcLength(frame.lower_contour, true) ==
            Approx(tc.lower_arc_length));
  }
}

TEST_CASE("test contour upright bounding rect", "[frame, contour]") {
  Frame frame{std::shared_ptr<cpptoml::table>{nullptr}};
  for (const auto& tc : tests) {
    auto f = cv::imread(frame_path(tc.frame));
    frame.Process(f);
    auto r = cv::boundingRect(frame.upper_contour);
    REQUIRE(cv::boundingRect(frame.upper_contour) == tc.upper_box);
    r = cv::boundingRect(frame.lower_contour);
    REQUIRE(cv::boundingRect(frame.lower_contour) == tc.lower_box);
  }
}
