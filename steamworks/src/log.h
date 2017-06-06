#pragma once

#include <cpptoml/cpptoml.h>
#include <spdlog/spdlog.h>

namespace steamworks {

class Log {
 public:
  static Log& GetInstance();
  static std::string GetTelemetryFilePath(std::string base);

  void Initialize(const std::shared_ptr<cpptoml::table> config);
  spdlog::level::level_enum GetLevel(std::string level);
  bool IsFlashDriveMounted();
  std::string GetLogFilePath();
  void Flush();

 private:
  bool is_initalized_ = false;
  spdlog::level::level_enum default_level_ = spdlog::level::off;

  Log();
};
} /* steamworks */
