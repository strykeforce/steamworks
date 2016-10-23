#include "deadeye/config.h"

#include <sys/stat.h>

#include <opencv2/opencv.hpp>
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

namespace spd = spdlog;

namespace deadeye {
bool file_exists(const std::string path) {
  struct stat buf;
  return (stat(path.c_str(), &buf) == 0);
}

Config::Config(std::string path) {
  if (!file_exists(path)) {
    throw std::runtime_error("config file not found.");
  }
  config_ = cpptoml::parse_file(path);

  ConfigureLogger();
}

Config::Config(int argc, char** argv) {
  if (argc < 2) {
    throw std::runtime_error("must specify config file as argument.");
  }
  Config(std::string(argv[1]));
}

std::shared_ptr<cpptoml::table> Config::GetTable(const std::string& table) {
  return config_->get_table(table);
}

void Config::ConfigureLogger() {
  auto console = spd::get("console");
  if (!console) {
    return;
  }
  auto level = config_->get_qualified_as<std::string>("logging.level");
  if (level->compare("trace") == 0) {
    console->set_level(spdlog::level::trace);
  } else if (level->compare("debug") == 0) {
    console->set_level(spdlog::level::debug);
  } else if (level->compare("info") == 0) {
    console->set_level(spdlog::level::info);
  } else if (level->compare("warn") == 0) {
    console->set_level(spdlog::level::warn);
  } else if (level->compare("err") == 0) {
    console->set_level(spdlog::level::err);
  } else if (level->compare("critical") == 0) {
    console->set_level(spdlog::level::critical);
  } else {
    console->warn("Unrecognized logging level {}, defaulting to warn", *level);
    console->set_level(spdlog::level::warn);
  }
  console->info("Logging level is {}", *level);
}

Config::~Config() {}
} /* deadeye */
