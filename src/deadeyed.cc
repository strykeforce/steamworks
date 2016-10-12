#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"

#include "deadeye_config.h"
#include "deadeye/config.h"
#include "deadeye/deadeye.h"

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
  console->info("Deadeye version {} is taking aim...", DEADEYE_VERSION);
  std::unique_ptr<deadeye::Deadeye> deadeye(new deadeye::Deadeye(config));
  deadeye->Start();
}
