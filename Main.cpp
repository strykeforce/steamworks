#include <stdio.h>
#include <sys/time.h>
#include <opencv2/opencv.hpp>
#include "UDPHandler.cpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace cv;
#include "NVIDIAUtils.cpp"

// 1280x720

#define TAN_DEG(__NUMBER__) /*  Takes in angle, returns slope  */ \
  tan(__NUMBER__ / 180.0 * M_PI)
#define ATAN2_DEG(__Y__, __X__) /*  Takes in slope, returns angle  */ \
  atan2(__Y__, __X__) * 180 / M_PI
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
void Scrollbar(int, void*) {}

Point inter(Point p1,
            double V11,
            double V12,
            Point p2,
            double V21,
            double V22) {
#define X1 ((double)p1.x)
#define Y1 ((double)p1.y)
#define X2 ((double)p2.x)
#define Y2 ((double)p2.y)
  // printf("inter(%f, %f, %f, %f, %f, %f, %f, %f)\n", X1, Y1, V11, V12, X2, Y2,
  // V21, V22);
  double num = -(V21 * Y1 - V22 * X1 - V21 * Y2 + V22 * X2);
  double dom = (V21 * V12 - V22 * V11);
  // printf("f == %f\n", num / dom);
  return Point(X1 + num / dom * V11, Y1 + num / dom * V12);
}
double AngleToFrom(double Direction, Point current, Point next) {
  // printf("AngleToFrom(%f, (%i, %i), (%i, %i));\n", Direction, current.x,
  // current.y, next.x, next.y);
  double angle = ATAN2_DEG(next.y - current.y, next.x - current.x);
  angle = ((int)angle + 360) % 360;
  // printf("angle == %f\n", angle);
  double option1 = (Direction) - (angle);
  // printf("option1  == %+f\n", option1);
  double option2 = 360 - fabs(option1);
  // printf("option2 == %+f\n", option2);
  double best = (abs(option1) < abs(option2)) ? option1 : option2;
  // printf("best == %f\n", best);
  return best;
}
#define Tol 50
#define Tol2 30
#define Tol_Points 4

Point everything(std::vector<Point> points,
                 int StartingIndex,
                 int Direction,
                 int Travel) {
  int NumberOfPoints = points.size();
  for (int i = 0;
       // i < 10;
       true; i += Travel) {
// printf("i == %i\n", i);
#define POINT_RELETIVE_TO(__INDEX__) \
  points[(StartingIndex + __INDEX__ + NumberOfPoints) % NumberOfPoints]
    double best = AngleToFrom(Direction, POINT_RELETIVE_TO(i),
                              POINT_RELETIVE_TO(i + Travel));
    bool yes = Tol > fabs(best);
    // printf("yes == %i\n", yes);
    if (!yes) {
      int I = 0;
      for (; I < Tol_Points; I++) {
        // printf("I == %i\n", I);
        double best2 = AngleToFrom(Direction, POINT_RELETIVE_TO(I * Travel + i),
                                   POINT_RELETIVE_TO(I * Travel + i + Travel));
        bool yes2 = Tol2 > fabs(best2);
        // printf("yes2 == %i\n", yes2);
        if (yes2) {
          i += I * Travel;
          break;
        }
      }
      if (I == Tol_Points)
        return POINT_RELETIVE_TO(i);
    }
  }
}

Scalar colors[] = {
    Scalar(255, 000, 000), Scalar(255, 128, 000), Scalar(255, 255, 000),
    Scalar(128, 255, 000), Scalar(000, 255, 128), Scalar(000, 255, 255),
    Scalar(000, 128, 255), Scalar(000, 000, 255), Scalar(128, 000, 255),
    Scalar(255, 000, 255), Scalar(255, 000, 128)};
int main(int argc, char** argv) {
  printf("Hello, World!\n");
  printf("getpid() == %i\n", getpid());
  usleep(1 * 1000 * 1000);
#define SEND_DATA_TO_ROBORIO 1
#if SEND_DATA_TO_ROBORIO
  SocketAddress* sock_addr = NULL;
  UDPSocket sock = UDPHandler::CreateUDPDataStreamNOBIND();
  printf("sock == %i\n", sock);
  while (sock_addr == NULL) {
    sock_addr = UDPHandler::CreateSocketAddress("10.27.67.2",
                                                NetworkP_Vision_Comm_Port);
    usleep(1 * 1000000);
    printf("sock_addr== %p\n", sock_addr);
  }
#endif
  int statis = system("touch /HelloWorld");
  NVIDIAUtils::ConfigCameraV4L2();
#define WIDTH 640.0
#define HEIGHT 480.0
#define X11 0
#define DRAW 0
#define WRITE_M5 0
#define WRITE_PATH "/home/ubuntu/pics"
  //#define WRITE_PATH	"/home/ubuntu/SD_CARD/pics"
  printf("st == %i\n", statis);
  VideoCapture vcap(0);
  vcap.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
  vcap.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);
  vcap.set(CV_CAP_PROP_BRIGHTNESS, 0.0);
  Mat m1, m2, m3, m4, m5;
#if X11
  namedWindow("m2", CV_WINDOW_AUTOSIZE);
  createTrackbar("H Max", "m2", &H_MAX, 400, Scrollbar);
  createTrackbar("H Min", "m2", &H_MIN, 360, Scrollbar);
  createTrackbar("S Max", "m2", &S_MAX, 360, Scrollbar);
  createTrackbar("S Min", "m2", &S_MIN, 360, Scrollbar);
  createTrackbar("L Max", "m2", &L_MAX, 360, Scrollbar);
  createTrackbar("L Min", "m2", &L_MIN, 360, Scrollbar);
#endif
  float UDPPacket[3];
#if WRITE_M5
  int pictureCount = 0;
  char* M5Pathname = (char*)malloc(sizeof(char) * 300);
// char* M5PathnameLog = (char*) malloc(sizeof(char) * 300);
// sprintf(M5PathnameLog, "%s/log.txt", WRITE_PATH);
// printf("M5PathnameLog == \"%s\"\n", M5PathnameLog);
// FILE* picFD = fopen(M5Pathname, "w");
// printf("picFD == %p\n", picFD);
#endif
  // while(waitKey(10) != ' ')
  struct timeval start, end;
  while (true) {
    gettimeofday(&end, NULL);
    printf("dt == %f\n", (double)(end.tv_sec - start.tv_sec) * 1000.0 +
                             (end.tv_usec - start.tv_usec) / 1000.0);
    gettimeofday(&start, NULL);
    vcap.read(m1);
    cv::inRange(m1, cv::Scalar(H_MIN / 1.0f, S_MIN / 1.0f, L_MIN / 1.0f),
                cv::Scalar(H_MAX / 1.0f, S_MAX / 1.0f, L_MAX / 1.0f), m2);
    cv::dilate(m2, m3, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
    cv::erode(m3, m4, cv::Mat(), cv::Point(-1, -1), 1, 1, 1);
#if DRAW
    cvtColor(m4, m5, cv::COLOR_GRAY2BGR);
#endif
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(m4.clone(), contours, CV_RETR_LIST,
                     CV_CHAIN_APPROX_SIMPLE);
    // printf("counts.size() == %i\n", contours.size());
    int BestIndex = -1;
    double BestSize = 0;

    // test for target is to find longest contour perimeter
    for (uint i = 0; i < contours.size(); i++) {
      // printf("counts[%i].size() == %i\n", i, contours[i].size());
      double CurrentSize = 0;
      for (uint KK = 0; KK < contours[i].size(); KK++) {
        int index2 = (KK + 1) % contours[i].size();
        // accumulate distance between current and next point in contour
        CurrentSize += sqrt(pow(contours[i][index2].x - contours[i][KK].x, 2) +
                            pow(contours[i][index2].y - contours[i][KK].y, 2));
      }
      // printf("CurrentSize == %f\n", CurrentSize);
      if (BestSize < CurrentSize) {
        BestSize = CurrentSize;
        BestIndex = i;
      }
      printf("contours[%i].size() == %f\n", i, CurrentSize);
// printf("%i, ", contours[i].size()); for(int I = 0;I<contours[i].size();I++)
// printf("%i, %i, ", contours[i][I].x, contours[i][I].y);
#if DRAW
      cv::drawContours(m5, contours, i, colors[i % 11]);
#endif
    }  // done finding longest target perimeter
#if X11
    imshow("m1", m1);
    imshow("m2", m2);
#endif
    printf("contours[%i].size() == %f\n", BestIndex, BestSize);

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
#if DRAW
      // draw a bounding box dialated by 20px
      rectangle(m5, Point(leftest.x - RECT_RADIUS, bottomest.y + RECT_RADIUS),
                Point(rightest.x + RECT_RADIUS, topest.y - RECT_RADIUS),
                Scalar(0, 255, 0), 2);
#endif
      // printf("leftest == (%i, %i)\n", leftest.x, leftest.y);
      // circle(m5, leftest,   4, Scalar(000, 000, 128), 1);
      // printf("rightest == (%i, %i)\n", rightest.x, rightest.y);
      // circle(m5, rightest,  4, Scalar(000, 128, 000), 1);
      // printf("bottomest == (%i, %i)\n", bottomest.x, bottomest.y);
      // circle(m5, bottomest, 5, Scalar(128, 000, 000), 1);

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
#if DRAW
      printf("LeftA  == (%i, %i)\n", LeftA.x, LeftA.y);
      circle(m5, LeftA, 4, Scalar(64, 64, 128), 1);
      printf("BottomB  == (%i, %i)\n", BottomB.x, BottomB.y);
      circle(m5, BottomB, 3, Scalar(128, 64, 64), 1);
      printf("BottomA  == (%i, %i)\n", BottomA.x, BottomA.y);
      circle(m5, BottomA, 3, Scalar(128, 64, 64), 1);
      printf("RightB  == (%i, %i)\n", RightB.x, RightB.y);
      circle(m5, RightB, 4, Scalar(64, 128, 64), 1);
      printf("RightA  == (%i, %i)\n", RightA.x, RightA.y);
      circle(m5, RightA, 4, Scalar(64, 128, 64), 1);
      printf("LeftB  == (%i, %i)\n", LeftB.x, LeftB.y);
      circle(m5, LeftB, 4, Scalar(64, 64, 128), 1);
      circle(m5, LeftBottom, 5, Scalar(128, 128, 128), 1);
      circle(m5, RightBottom, 5, Scalar(128, 128, 128), 1);
#endif
// imshow("m3", m3);
// imshow("m4", m4);
#if X11
      imshow("m5", m5);
#endif
#if WRITE_M5
      sprintf(M5Pathname, "%s/I%i.png", WRITE_PATH, pictureCount);
      printf("M5Pathname == %s\n", M5Pathname);
      imwrite(M5Pathname, m5);
      pictureCount++;
      printf("pictureCount == %i\n", pictureCount);
      sprintf(M5Pathname, "%s/O%i.png", WRITE_PATH, pictureCount);
      printf("M5Pathname == %s\n", M5Pathname);
      imwrite(M5Pathname, m1);
      pictureCount++;
      printf("pictureCount == %i\n", pictureCount);
#endif
      int InterAX = (LeftBottom.x + RightBottom.x) / 2;
      printf("Aver.X == %i\n", InterAX);
      double Center = WIDTH / 2 - InterAX;
      printf("Dist To Center == %f\n", Center);
      double LineLength = sqrt(pow(LeftBottom.x - RightBottom.x, 2) +
                               pow(LeftBottom.y - RightBottom.y, 2));
      printf("Line Length == %f\n", LineLength);
      // double P = tan( (LineLength / 2 * 0.1032) / 180 * M_PI);
      // printf("P == %f\n", P);
      // 0.1032
      // double PixelToThing = 0.090;
      double PixelToThing = 0.085;
      // double PixelToThing = 0.0866;
      double DistInches = (double)(20.0 / 2.0) /
                          (tan(LineLength / 2 * PixelToThing * M_PI / 180));
      printf("DistInches  == %f\n", DistInches);
      printf("%f\n", PixelToThing);
#if WRITE_M5
// printf(".\n");
// fprintf(picFD, "\"%s\", Dist to center == %f, DistInches == %f\n",
// M5PathnameLog, Center, DistInches);
// fflush(picFD);
#endif
#if SEND_DATA_TO_ROBORIO
      UDPPacket[0] = Center;
      UDPPacket[1] = DistInches;
      UDPPacket[2] = 0;
#endif
    } else {
      UDPPacket[2] = 42;
    }
    UDPHandler::UDPSend(sock, UDPPacket, sizeof(float) * 3, sock_addr);
    // usleep(10 * 1000);
  }
}
