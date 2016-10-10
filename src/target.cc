#include "deadeye/target.h"

#include <opencv2/opencv.hpp>

#define ATAN2_DEG(__Y__, __X__) /*  Takes in slope, returns angle  */ \
  atan2(__Y__, __X__) * 180 / M_PI

cv::Point inter(cv::Point p1,
                double V11,
                double V12,
                cv::Point p2,
                double V21,
                double V22) {
#define X1 ((double)p1.x)
#define Y1 ((double)p1.y)
#define X2 ((double)p2.x)
#define Y2 ((double)p2.y)
  double num = -(V21 * Y1 - V22 * X1 - V21 * Y2 + V22 * X2);
  double dom = (V21 * V12 - V22 * V11);
  return cv::Point(X1 + num / dom * V11, Y1 + num / dom * V12);
}

double AngleToFrom(double direction, cv::Point current, cv::Point next) {
  double angle = ATAN2_DEG(next.y - current.y, next.x - current.x);
  angle = ((int)angle + 360) % 360;
  double option1 = (direction) - (angle);
  double option2 = 360 - fabs(option1);
  double best = (std::abs(option1) < std::abs(option2)) ? option1 : option2;
  return best;
}

#define Tol 50
#define Tol2 30
#define Tol_Points 4

cv::Point corner_point(std::vector<cv::Point> points,
                       int start_index,
                       int direction,
                       int travel) {
  int NumberOfPoints = points.size();
  for (int i = 0;
       // i < 10;
       true; i += travel) {
#define POINT_RELETIVE_TO(__INDEX__) \
  points[(start_index + __INDEX__ + NumberOfPoints) % NumberOfPoints]
    double best = AngleToFrom(direction, POINT_RELETIVE_TO(i),
                              POINT_RELETIVE_TO(i + travel));
    bool yes = Tol > fabs(best);
    if (!yes) {
      int I = 0;
      for (; I < Tol_Points; I++) {
        double best2 = AngleToFrom(direction, POINT_RELETIVE_TO(I * travel + i),
                                   POINT_RELETIVE_TO(I * travel + i + travel));
        bool yes2 = Tol2 > fabs(best2);
        if (yes2) {
          i += I * travel;
          break;
        }
      }
      if (I == Tol_Points)
        return POINT_RELETIVE_TO(i);
    }
  }
}
