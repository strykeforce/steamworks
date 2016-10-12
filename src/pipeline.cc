#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"

#include "deadeye/camera.h"
#include "deadeye/config.h"
#include "deadeye/frame_processor.h"
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
  std::unique_ptr<deadeye::FrameProcessor> frame_processor(
      new deadeye::FrameProcessor(config));

  namedWindow(window_name, cv::WINDOW_AUTOSIZE);
  console->info("Press <ESC> to exit.");

  for (;;) {
    cv::Mat frame;
    camera->Read(frame);

    frame_processor->TargetContour(frame);

    cv::imshow(window_name, frame_processor->dilated_frame);

    auto c = (char)cvWaitKey(1);
    if (c == 27)
      break;
  }
  cv::destroyWindow(window_name);
  return 0;
}
