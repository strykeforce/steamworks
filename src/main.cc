#include <cstdlib>
#include <chrono>
#include <thread>

#include "spdlog/spdlog.h"
#include "cpptoml/cpptoml.h"

// #include "boiler_camera.h"
#include "deadeye.h"

namespace {
constexpr auto STARTUP_DELAY_SEC = std::chrono::seconds(10);
}

int main(int argc, char const* argv[]) {
#if NDEBUG
  auto logger = spdlog::stdout_logger_st("deadeye");
  logger->set_level(spdlog::level::info);
  spdlog::set_pattern("[%n][%l] %v");
#else
  auto logger = spdlog::stdout_logger_st("deadeye");
  logger->set_level(spdlog::level::trace);
  spdlog::set_pattern("[%H:%M:%S.%e][%n][%l] %v");
#endif
  // read config file from path specified in DEADEYE_CONF env variable or
  // default to /etc/deadeye.toml
  const char* conf_path = std::getenv("DEADEYE_CONF");
  if (!conf_path) {
    conf_path = "/etc/deadeye/deadeye.toml";
  }
  logger->info("reading configuration from {}", conf_path);
  auto config = cpptoml::parse_file(conf_path);

  bool has_display = !!std::getenv("DISPLAY");
  if (!has_display) {
    std::this_thread::sleep_for(STARTUP_DELAY_SEC);
  }
  // start processing
  deadeye::Deadeye deadeye(config);
  deadeye.Run();
  return 0;
}
