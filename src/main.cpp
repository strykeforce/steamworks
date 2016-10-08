#include <fcntl.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "spdlog/spdlog.h"
#include "cpptoml.h"
#include <opencv2/opencv.hpp>

#include "constants.h"
#include "nvidia_utils.h"
#include "target.h"
#include "udp_handler.h"

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
  SocketAddress* sock_addr = NULL;
  UDPSocket sock = CreateUDPDataStreamNOBIND();
  console->debug("sock == {}", sock);
  while (sock_addr == NULL) {
    sock_addr = CreateSocketAddress("127.0.0.1", kNetworkVisionCommPort);
    usleep(1 * 1000000);
  }

  ConfigCameraV4L2();
#define WIDTH 640.0
#define HEIGHT 480.0

  VideoCapture vcap(0);
  vcap.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
  vcap.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);
  vcap.set(CV_CAP_PROP_BRIGHTNESS, 0.0);
  Mat m1, m2, m3, m4, m5;

  float UDPPacket[3];
  struct timeval start, end;
  while (true) {
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
    int BestIndex = -1;
    double BestSize = 0;

    // test for target is to find longest contour perimeter
    for (uint i = 0; i < contours.size(); i++) {
      console->debug("counts[{}].size() == {}", i, contours[i].size());
      double CurrentSize = 0;
      for (uint KK = 0; KK < contours[i].size(); KK++) {
        int index2 = (KK + 1) % contours[i].size();
        // accumulate distance between current and next point in contour
        CurrentSize += sqrt(pow(contours[i][index2].x - contours[i][KK].x, 2) +
                            pow(contours[i][index2].y - contours[i][KK].y, 2));
      }
      if (BestSize < CurrentSize) {
        BestSize = CurrentSize;
        BestIndex = i;
      }
      console->debug("contours[{}].size() == {}", i, CurrentSize);
    }  // done finding longest target perimeter

    console->debug("contours[{}].size() == {}", BestIndex, BestSize);

    // punt on this frame if not large enough perimeter
    if (BestSize > 250) {
#define points contours[BestIndex]
      Point leftest = points[0], rightest = leftest, bottomest = leftest,
            topest = leftest;
      int leftestIndex = 0, rightestIndex = leftestIndex,
          bottomestIndex = leftestIndex;

      // find left, top, right and bottom-most contour points
      for (uint i = 1; i < contours[BestIndex].size(); i++) {
        if (contours[BestIndex][i].x < leftest.x) {
          leftest = points[i];
          leftestIndex = i;
        }
        if (contours[BestIndex][i].x > rightest.x) {
          rightest = points[i];
          rightestIndex = i;
        }
        if (contours[BestIndex][i].y > bottomest.y) {
          bottomest = points[i];
          bottomestIndex = i;
        }
        if (contours[BestIndex][i].y < topest.y) {
          topest = points[i];
        }
      }

#define RECT_RADIUS 20

      Point LeftA = everything(points, leftestIndex, 90, 1);
      Point LeftB = everything(points, leftestIndex, 270, -1);
      Point RightA = everything(points, rightestIndex, 270, 1);
      Point RightB = everything(points, rightestIndex, 90, -1);
      Point BottomA = everything(points, bottomestIndex, 180, -1);
      Point BottomB = everything(points, bottomestIndex, 0, 1);
      Point LeftBottom =
          inter(LeftA, LeftA.x - LeftB.x, LeftA.y - LeftB.y, BottomA,
                BottomA.x - BottomB.x, BottomA.y - BottomB.y);
      Point RightBottom =
          inter(RightA, RightA.x - RightB.x, RightA.y - RightB.y, BottomA,
                BottomA.x - BottomB.x, BottomA.y - BottomB.y);
      int InterAX = (LeftBottom.x + RightBottom.x) / 2;
      console->info("Aver.X == {}", InterAX);
      double Center = WIDTH / 2 - InterAX;
      console->info("Dist To Center == {}", Center);
      double LineLength = sqrt(pow(LeftBottom.x - RightBottom.x, 2) +
                               pow(LeftBottom.y - RightBottom.y, 2));
      console->info("Line Length == {}", LineLength);
      double PixelToThing = 0.085;
      double DistInches = (double)(20.0 / 2.0) /
                          (tan(LineLength / 2 * PixelToThing * M_PI / 180));
      console->info("DistInches  == {}", DistInches);
      console->info("PixelToThing = {}", PixelToThing);
      UDPPacket[0] = Center;
      UDPPacket[1] = DistInches;
      UDPPacket[2] = 0;
    } else {
      UDPPacket[2] = 42;
    }
    UDPSend(sock, UDPPacket, sizeof(float) * 3, sock_addr);
    // usleep(10 * 1000);
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
  console->info("Deadeye PID == {}", getpid());

  usleep(1 * 1000 * 1000);
  start();
}
