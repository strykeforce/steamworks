#ifndef DEADEYE_TARGET_INC_
#define DEADEYE_TARGET_INC_

#include <opencv2/opencv.hpp>

cv::Point inter(cv::Point p1,
                double V11,
                double V12,
                cv::Point p2,
                double V21,
                double V22);

double AngleToFrom(double direction, cv::Point current, cv::Point next);

cv::Point everything(std::vector<cv::Point> points,
                     int start_index,
                     int direction,
                     int travel);

#endif /* end of include guard: DEADEYE_TARGET_INC_ */
