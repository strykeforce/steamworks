#include "log.h"

#include <spdlog/spdlog.h>

using namespace steamworks;
using namespace spdlog;

/**
 * Configure logging based on release type.
 * If building for Release, don't put ANSI terminal color codes in log since
 * it will be read in driver station logs.
 */
void Log::Initialize(const std::shared_ptr<cpptoml::table> config) {
#ifdef NDEBUG
  auto logger = stdout_logger_st("robot");
  auto level = level::info;
  logger->set_level(level);
  stdout_logger_st("command")->set_level(level);
  stdout_logger_st("subsystem")->set_level(level);
  stdout_logger_st("sidewinder")->set_level(level);
  stdout_logger_mt("deadeye")->set_level(level);
#else
  auto logger = stdout_logger_st("robot");
  auto level = level::trace;
  logger->set_level(level);
  stdout_color_st("command")->set_level(level);
  stdout_color_st("subsystem")->set_level(level);
  stdout_color_st("sidewinder")->set_level(level::info);
  stdout_color_mt("deadeye")->set_level(level);
#endif
  set_pattern("[%H:%M:%S.%e][%n][%l] %v");
}
