#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"

#include "deadeye/config.h"
#include "deadeye/deadeye.h"

namespace spd = spdlog;

const char* frame_window = "Captured Frame";
const char* mask_window = "Mask";

int main(int argc, char** argv) {
  auto console = spd::stdout_logger_st("console");
  // TODO: look at factory method for config, try/catch is clumsy
  std::shared_ptr<deadeye::Config> config;

  try {
    // TODO: what is better for constructor args for Config?
    config = std::make_shared<deadeye::Config>(argc, argv);
  } catch (const std::exception& e) {
    console->critical("Config file error: {}", e.what());
    return 1;
  }

  std::unique_ptr<deadeye::Deadeye> deadeye(new deadeye::Deadeye(config));
  cv::namedWindow(frame_window, cv::WINDOW_AUTOSIZE);
  cv::moveWindow(frame_window, 0, 0);
  cv::namedWindow(mask_window, cv::WINDOW_AUTOSIZE);
  cv::moveWindow(mask_window, 640, 0);
  console->info("Press <ESC> to exit.");

  deadeye->SetFrameCallback([](const cv::Mat& frame) -> bool {
    cv::imshow(frame_window, frame);
    auto c = (char)cv::waitKey(1);
    return c == 27;
  });

  deadeye->SetMaskCallback([](const cv::Mat& mask) -> bool {
    cv::imshow(mask_window, mask);
    auto c = (char)cv::waitKey(1);
    return c == 27;
  });

  deadeye->Start();

  cv::destroyAllWindows();
  return 0;
}
