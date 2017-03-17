#include "log.h"

using namespace steamworks;
using namespace spdlog;
using namespace std;

Log& Log::GetInstance() {
  static Log log;
  return log;
}

/**
 * private
 */
Log::Log() {}

/**
 * Configure logging based on release type.
 * If building for Release, don't put ANSI terminal color codes in log since
 * it will be read in driver station logs.
 */
void Log::Initialize(const std::shared_ptr<cpptoml::table> config_in) {
  if (is_initalized_) {
    get("robot")->error("Log class should only be intialized once.");
    return;
  }
  auto logger = spdlog::stdout_logger_st("log");

  auto config = config_in->get_table("LOGGING");
  bool color = config->get_as<bool>("color").value_or(false);
  default_level_ =
      GetLevel(config->get_as<string>("default").value_or("trace"));

  auto loggers = config->get_array_of<string>("loggers");
  if (!loggers) {
    logger->warn("LOGGING loggers setting missing, skipping configuration");
    return;
  }
  for (const auto& log : *loggers) {
    // is the default logging level overidden for this log?
    auto level_name = config->get_as<string>(log);
    level::level_enum level = default_level_;
    if (level_name) {
      level = GetLevel(*level_name);
    }

    logger->info("configuring {} at level {}", log, level::to_str(level));
    // use ANSI color logging?
    if (color) {
      stdout_color_st(log)->set_level(level);
      continue;
    }
    stdout_logger_st(log)->set_level(level);
  }

  set_pattern("[%H:%M:%S.%e][%n][%l] %v");
  is_initalized_ = true;
}

/**
 * Find the enum value corresponding the to level name. Level enum and name
 * array are defined in spdlog/common.h.
 */
level::level_enum Log::GetLevel(const std::string level) {
  using namespace level;
  auto result = find(begin(level_names), end(level_names), level);
  if (result != end(level_names)) {
    return static_cast<level_enum>(distance(begin(level_names), result));
  } else {
    return default_level_;
  }
}
