#include "config.h"

#include <sys/stat.h>

#include <opencv2/opencv.hpp>
#include "cpptoml.h"
#include "spdlog/spdlog.h"

namespace spd = spdlog;

namespace deadeye {
bool file_exists(const char* path) {
  struct stat buf;
  return (stat(path, &buf) == 0);
}

Config::Config(int argc, char** argv) {
  if (argc < 2) {
    throw std::runtime_error("must specify config file as argument.");
  }

  auto path = argv[1];

  if (!file_exists(path)) {
    throw std::runtime_error("config file not found.");
  }
  try {
    config_ = cpptoml::parse_file(path);
  } catch (const cpptoml::parse_exception& e) {
    throw std::runtime_error(e.what());
  }
  ConfigureLogger();
  ConfigureRobot();
  ConfigureColor();
}

void Config::ConfigureLogger() {
  auto level = config_->get_qualified_as<std::string>("logging.level");
  auto console = spd::get("console");
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

void Config::ConfigureRobot() {
  host = (config_->get_qualified_as<std::string>("robot.address"))->c_str();
  port = (config_->get_qualified_as<std::string>("robot.port"))->c_str();
}

void Config::ConfigureColor() {
  auto console = spd::get("console");
  auto color = config_->get_table("color");
  auto hue = color->get_qualified_as<double>("lower.hue");
  auto sat = color->get_qualified_as<double>("lower.saturation");
  auto val = color->get_qualified_as<double>("lower.value");
  if (hue && sat && val) {
    range_lower = cv::Scalar(*hue, *sat, *val);
  } else {
    console->error("config error: color.lower");
  }

  hue = color->get_qualified_as<double>("upper.hue");
  sat = color->get_qualified_as<double>("upper.saturation");
  val = color->get_qualified_as<double>("upper.value");
  if (hue && sat && val) {
    range_upper = cv::Scalar(*hue, *sat, *val);
  } else {
    console->error("config error: color.upper");
  }

  console->debug("config->range_lower = ({}, {}, {})", range_lower[0],
                 range_lower[1], range_lower[2]);
  console->debug("config->range_upper = ({}, {}, {})", range_upper[0],
                 range_upper[1], range_upper[2]);
}

Config::~Config() {}
} /* deadeye */
