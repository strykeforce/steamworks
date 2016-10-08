#include "spdlog/spdlog.h"

#include <unistd.h>
#include <opencv2/highgui/highgui.hpp>

namespace spd = spdlog;

class NVIDIAUtils {
 public:
  static void ConfigCameraV4L2() {
    auto console = spd::get("console");
    console->debug("NVIDIAUtils::ConfigCameraV4L2()");
    int status1, status2, status3, status4;
    do {
      status1 = system("uvcdynctrl -s Exposure,\\ Auto 1");
      console->debug("status2 == {}", status1);
      status2 = system("uvcdynctrl -s Exposure\\ \\(Absolute\\) 10");
      console->debug("status1 == {}", status2);
      status3 = system("uvcdynctrl -s White\\ Balance\\ Temperature,\\ Auto 0");
      console->debug("status4 == {}", status3);
      status4 = system("uvcdynctrl -s White\\ Balance\\ Temperature 4500");
      console->debug("status3 == {}", status4);
      usleep(1 * 1000 * 1000);
    } while (status1 | status2 | status3 | status4);
  }
  static void PrintCameraValues(cv::VideoCapture vcap) {
    auto console = spd::get("console");
    console->debug("NVIDIAUtils::PrintCameraValues()");
    console->debug("CV_CAP_PROP_FRAME_WIDTH == {}",
                  vcap.get(CV_CAP_PROP_FRAME_WIDTH));
    console->debug("CV_CAP_PROP_FRAME_HEIGHT == {}",
                  vcap.get(CV_CAP_PROP_FRAME_HEIGHT));
    console->debug("CV_CAP_PROP_FPS == {}", vcap.get(CV_CAP_PROP_FPS));
    console->debug("CV_CAP_PROP_BRIGHTNESS == {}",
                  vcap.get(CV_CAP_PROP_BRIGHTNESS));
    console->debug("CV_CAP_PROP_CONTRAST == {}", vcap.get(CV_CAP_PROP_CONTRAST));
    console->debug("CV_CAP_PROP_SATURATION == {}",
                  vcap.get(CV_CAP_PROP_SATURATION));
    console->debug("CV_CAP_PROP_MODE == {}", vcap.get(CV_CAP_PROP_MODE));
  }
};
