#include <cstdlib>
#include <chrono>
#include <thread>

#include "spdlog/spdlog.h"
#include "cpptoml/cpptoml.h"

// #include "boiler_camera.h"
#include "deadeye.h"

int main(int argc, char const* argv[]) {
  auto logger = spdlog::stdout_logger_st("deadeye");
  spdlog::stdout_logger_st("boiler");
  spdlog::stdout_logger_st("gear");
  spdlog::stdout_logger_st("link");
  spdlog::set_pattern("[%S.%e][%n][%l] %v");
#if NDEBUG
  spdlog::set_level(spdlog::level::info);
#else
  spdlog::set_level(spdlog::level::trace);
#endif
  // read config file from path specified in DEADEYE_CONF env variable or
  // default to /etc/deadeye.toml
  const char* conf_path = std::getenv("DEADEYE_CONF");
  if (!conf_path) {
    conf_path = "/etc/deadeye/deadeye.toml";
  }
  logger->info("reading configuration from {}", conf_path);
  auto config = cpptoml::parse_file(conf_path);

  // start processing
  deadeye::Deadeye deadeye(config);
  deadeye.Run();
  return 0;
}
