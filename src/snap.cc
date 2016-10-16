#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"

#include "lib/config.h"
#include "lib/deadeye.h"

namespace spd = spdlog;

int main(int argc, char** argv) {
  auto console = spd::stdout_logger_st("console", true);
  std::shared_ptr<deadeye::Config> config;

  try {
    config = std::make_shared<deadeye::Config>(argc, argv);
  } catch (const std::exception& e) {
    console->critical("Config file error: {}", e.what());
    return 1;
  }

  std::unique_ptr<deadeye::Deadeye> deadeye(new deadeye::Deadeye(config));

  deadeye->SetFrameCallback([](const cv::Mat& frame) -> bool {
    cv::imwrite("snap.jpg", frame);
    return true;
  });

  deadeye->Start();
}
