#pragma once

#include <vector>
#include <string>
#include "opencv2/opencv.hpp"

struct FrameTestCase {
  std::string frame;
  double upper_arc_length, lower_arc_length;
  cv::Rect upper_box, lower_box;
};

std::vector<FrameTestCase> tests = {
    {
        "data/boiler_000.png",
        356.35,
        321.72,
        {201, 290, 119, 57},  // 119 x 57 from (201, 290)
        {191, 354, 118, 41},  // 118 x 41 from (191, 354)
    },
};
