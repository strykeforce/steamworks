#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"

#include "deadeye/camera.h"
#include "deadeye/config.h"
#include "deadeye/deadeye.h"
#include "deadeye/target.h"

namespace spd = spdlog;

const char* window_name = "Deadeye";

int main(int argc, char** argv) {
  auto console = spd::stdout_logger_st("console", true);
  // TODO: look at factory method for config, try/catch is clumsy
  std::shared_ptr<deadeye::Config> config;

  try {
    // TODO: what is better for constructor args for Config?
    config = std::make_shared<deadeye::Config>(argc, argv);
  } catch (const std::exception& e) {
    console->critical("Config file error: {}", e.what());
    return 1;
  }

  std::unique_ptr<deadeye::Camera> camera(new deadeye::Camera(config));
  std::unique_ptr<deadeye::Deadeye> deadeye(new deadeye::Deadeye(config));

  namedWindow(window_name, cv::WINDOW_AUTOSIZE);
  console->info("Press <ESC> to exit.");

  for (;;) {
    cv::Mat frame;
    camera->Read(frame);

    deadeye->TargetContour(frame);

    cv::imshow(window_name, deadeye->dilated_frame);

    auto c = (char)cvWaitKey(1);
    if (c == 27)
      break;
  }
  cv::destroyAllWindows();
  return 0;
}
