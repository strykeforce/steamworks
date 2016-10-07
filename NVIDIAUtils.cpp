#include <unistd.h>
#include <opencv2/highgui/highgui.hpp>

#ifndef NID_Printout
#define NID_Printout 1
#endif

class NVIDIAUtils {
 public:
  static void ConfigCameraV4L2() {
#if NID_Printout
    printf("NVIDIAUtils::ConfigCameraV4L2()\n");
#endif
    int status1, status2, status3, status4;
    do {
      status1 = system("uvcdynctrl -s Exposure,\\ Auto 1");
      printf("status2 == %i\n", status1);
      status2 = system("uvcdynctrl -s Exposure\\ \\(Absolute\\) 10");
      printf("status1 == %i\n", status2);
      status3 = system("uvcdynctrl -s White\\ Balance\\ Temperature,\\ Auto 0");
      printf("status4 == %i\n", status3);
      status4 = system("uvcdynctrl -s White\\ Balance\\ Temperature 4500");
      printf("status3 == %i\n", status4);
      usleep(1 * 1000 * 1000);
    } while (status1 | status2 | status3 | status4);
  }
  static void PrintCameraValues(cv::VideoCapture vcap) {
#if NID_Printout
    printf("NVIDIAUtils::PrintCameraValues()\n");
#endif
    printf("CV_CAP_PROP_FRAME_WIDTH == %f\n",
           vcap.get(CV_CAP_PROP_FRAME_WIDTH));
    printf("CV_CAP_PROP_FRAME_HEIGHT == %f\n",
           vcap.get(CV_CAP_PROP_FRAME_HEIGHT));
    printf("CV_CAP_PROP_FPS == %f\n", vcap.get(CV_CAP_PROP_FPS));
    printf("CV_CAP_PROP_BRIGHTNESS == %f\n", vcap.get(CV_CAP_PROP_BRIGHTNESS));
    printf("CV_CAP_PROP_CONTRAST == %f\n", vcap.get(CV_CAP_PROP_CONTRAST));
    printf("CV_CAP_PROP_SATURATION == %f\n", vcap.get(CV_CAP_PROP_SATURATION));
    printf("CV_CAP_PROP_MODE == %f\n", vcap.get(CV_CAP_PROP_MODE));
    // printf("CV_CAP_PROP_WHITE_BALANCE_U == %f\n",
    // vcap.get(CV_CAP_PROP_WHITE_BALANCE_U)); stub!
    // printf("CV_CAP_PROP_WHITE_BALANCE_V == %f\n",
    // vcap.get(CV_CAP_PROP_WHITE_BALANCE_V)); stub!
  }
};
