#include "log.h"

#include <mntent.h>
#include <cstdio>
#include <cstring>
#include <ctime>

using namespace steamworks;
using namespace spdlog;
using namespace std;

namespace {
const char* kFlashDriveMount = "/";
const char* kLocalPath = "/home/lvuser/logs/";
const char* kFlashDrivePath = kLocalPath;
}

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
  bool is_file = config->get_as<bool>("file").value_or(false);
  default_level_ =
      GetLevel(config->get_as<string>("default").value_or("trace"));

  auto loggers = config->get_array_of<string>("loggers");
  if (!loggers) {
    logger->warn("LOGGING loggers setting missing, skipping configuration");
    return;
  }

  shared_ptr<sinks::simple_file_sink_mt> file_sink;
  if (is_file) {
    file_sink = make_shared<sinks::simple_file_sink_mt>(GetLogFilePath());
  }

  for (const auto& log : *loggers) {
    // is the default logging level overidden for this log?
    auto level_name = config->get_as<string>(log);
    level::level_enum level = default_level_;
    if (level_name) {
      level = GetLevel(*level_name);
    }

    logger->info("configuring {} at level {}", log, level::to_str(level));

    if (is_file) {
      auto flog = make_shared<spdlog::logger>(log, file_sink);
      flog->set_level(level);
      register_logger(flog);
      continue;
    }
    stdout_color_st(log)->set_level(level);
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

/**
* Returns true if a flash drive is mounted.
*/
bool Log::IsFlashDriveMounted() {
  FILE* mtab = NULL;
  mntent* part = NULL;
  bool is_mounted = false;
  if ((mtab = setmntent(_PATH_MOUNTED, "r")) != NULL) {
    while ((part = getmntent(mtab)) != NULL) {
      if ((part->mnt_dir != NULL) &&
          (strcmp(part->mnt_dir, kFlashDriveMount) == 0)) {
        is_mounted = true;
        break;
      }
    }
    endmntent(mtab);
  }
  return is_mounted;
}

/**
* Create a log file name from timestamp.
*/
string Log::GetLogFilePath() {
  char ts[16];
  ostringstream name;
  const char* dir = IsFlashDriveMounted() ? kFlashDrivePath : kLocalPath;
  time_t t = std::time(nullptr);
  if (strftime(ts, sizeof(ts), "%Y%m%d_%H%M%S", localtime(&t))) {
    name << dir << ts << ".log";
  } else {
    name << dir << "steamworks.log";
  }
  return name.str();
}
