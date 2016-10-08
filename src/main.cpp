#include <sys/time.h>

#include "spdlog/spdlog.h"
#include "cpptoml.h"
#include <opencv2/opencv.hpp>

#include "constants.h"
#include "message.h"
#include "nvidia_utils.h"
#include "target.h"

namespace spd = spdlog;
using namespace cv;

// 1280x720

#define CAMERA_NUMBERS 1
#if CAMERA_NUMBERS
int H_MAX = 125, H_MIN = 65;
int S_MAX = 180, S_MIN = 105;
int L_MAX = 90, L_MIN = 0;
/*
        int H_MAX = 108, H_MIN = 57;
        int S_MAX = 161, S_MIN = 87;
        int L_MAX =  70, L_MIN = 14;
*/
#else
int H_MAX = 140, H_MIN = 55;
int S_MAX = 190, S_MIN = 90;
int L_MAX = 75, L_MIN = 0;
#endif

void start() {
  auto console = spd::get("console");
  auto message = new deadeye::Message("127.0.0.1", "5804");
  float payload[3];

  ConfigCameraV4L2();
#define WIDTH 640.0
#define HEIGHT 480.0

  VideoCapture vcap(0);
  vcap.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
  vcap.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);
  vcap.set(CV_CAP_PROP_BRIGHTNESS, 0.0);
  Mat m1, m2, m3, m4, m5;

  struct timeval start, end;
  for (;;) {
    gettimeofday(&end, NULL);
    console->debug("dt == {}", (double)(end.tv_sec - start.tv_sec) * 1000.0 +
                                   (end.tv_usec - start.tv_usec) / 1000.0);
    gettimeofday(&start, NULL);
    vcap.read(m1);
    cv::inRange(m1, cv::Scalar(H_MIN / 1.0f, S_MIN / 1.0f, L_MIN / 1.0f),
                cv::Scalar(H_MAX / 1.0f, S_MAX / 1.0f, L_MAX / 1.0f), m2);
    cv::dilate(m2, m3, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
    cv::erode(m3, m4, cv::Mat(), cv::Point(-1, -1), 1, 1, 1);
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(m4.clone(), contours, CV_RETR_LIST,
                     CV_CHAIN_APPROX_SIMPLE);
    console->debug("counts.size() == {}", contours.size());
    int best_index = -1;
    double best_size = 0;

    // test for target is to find longest contour perimeter
    for (uint i = 0; i < contours.size(); i++) {
      console->debug("counts[{}].size() == {}", i, contours[i].size());
      double current_size = 0;
      for (uint KK = 0; KK < contours[i].size(); KK++) {
        int index2 = (KK + 1) % contours[i].size();
        // accumulate distance between current and next point in contour
        current_size += sqrt(pow(contours[i][index2].x - contours[i][KK].x, 2) +
                             pow(contours[i][index2].y - contours[i][KK].y, 2));
      }
      if (best_size < current_size) {
        best_size = current_size;
        best_index = i;
      }
      console->debug("contours[{}].size() == {}", i, current_size);
    }  // done finding longest target perimeter

    console->debug("contours[{}].size() == {}", best_index, best_size);

    // punt on this frame if not large enough perimeter
    if (best_size > 250) {
#define points contours[best_index]
      Point leftest = points[0], rightest = leftest, bottomest = leftest,
            topest = leftest;
      int leftest_index = 0, rightest_index = leftest_index,
          bottomest_index = leftest_index;

      // find left, top, right and bottom-most contour points
      for (uint i = 1; i < contours[best_index].size(); i++) {
        if (contours[best_index][i].x < leftest.x) {
          leftest = points[i];
          leftest_index = i;
        }
        if (contours[best_index][i].x > rightest.x) {
          rightest = points[i];
          rightest_index = i;
        }
        if (contours[best_index][i].y > bottomest.y) {
          bottomest = points[i];
          bottomest_index = i;
        }
        if (contours[best_index][i].y < topest.y) {
          topest = points[i];
        }
      }

#define RECT_RADIUS 20

      Point left_a = everything(points, leftest_index, 90, 1);
      Point left_b = everything(points, leftest_index, 270, -1);
      Point right_a = everything(points, rightest_index, 270, 1);
      Point right_b = everything(points, rightest_index, 90, -1);
      Point bottom_a = everything(points, bottomest_index, 180, -1);
      Point bottom_b = everything(points, bottomest_index, 0, 1);
      Point left_bottom =
          inter(left_a, left_a.x - left_b.x, left_a.y - left_b.y, bottom_a,
                bottom_a.x - bottom_b.x, bottom_a.y - bottom_b.y);
      Point right_bottom =
          inter(right_a, right_a.x - right_b.x, right_a.y - right_b.y, bottom_a,
                bottom_a.x - bottom_b.x, bottom_a.y - bottom_b.y);
      int inter_ax = (left_bottom.x + right_bottom.x) / 2;
      console->info("inter_ax == {}", inter_ax);
      double center = WIDTH / 2 - inter_ax;
      console->info("Dist To center == {}", center);
      double line_length = sqrt(pow(left_bottom.x - right_bottom.x, 2) +
                                pow(left_bottom.y - right_bottom.y, 2));
      console->info("Line Length == {}", line_length);
      double pixel_to_thing = 0.085;
      double dist_inches = (double)(20.0 / 2.0) /
                           (tan(line_length / 2 * pixel_to_thing * M_PI / 180));
      console->info("dist_inches  == {}", dist_inches);
      console->info("pixel_to_thing = {}", pixel_to_thing);
      payload[0] = center;
      payload[1] = dist_inches;
      payload[2] = 0;
    } else {
      payload[2] = 42;
    }
    message->send(payload);
  }
}

int main(int argc, char** argv) {
  auto console = spd::stdout_logger_st("console", true);
  auto config = cpptoml::parse_file("config/config.toml");
  auto val = config->get_qualified_as<std::string>("logging.level");
  if (val->compare("trace") == 0) {
    console->set_level(spdlog::level::trace);
  } else if (val->compare("debug") == 0) {
    console->set_level(spdlog::level::debug);
  } else if (val->compare("info") == 0) {
    console->set_level(spdlog::level::info);
  } else if (val->compare("warn") == 0) {
    console->set_level(spdlog::level::warn);
  } else if (val->compare("err") == 0) {
    console->set_level(spdlog::level::err);
  } else if (val->compare("critical") == 0) {
    console->set_level(spdlog::level::critical);
  } else {
    console->warn("Unrecognized logging level {}, defaulting to warn", *val);
    console->set_level(spdlog::level::warn);
  }
  console->info("Deadeye is taking aim...");
  console->info("Logging level is {}", *val);

  start();
}
