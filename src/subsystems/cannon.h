#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

namespace avenger {

class Cannon : public ::Subsystem {
 private:
  const std::shared_ptr<spdlog::logger> logger_;
  std::vector<std::unique_ptr<::Solenoid>> solenoids_;
  int fire_duration_ms_ = 0;
  int barrel_ = 0;

 public:
  Cannon(const std::shared_ptr<cpptoml::table> config);
  Cannon(const Cannon&) = delete;
  Cannon& operator=(const Cannon&) = delete;
  virtual ~Cannon() = default;

  void Fire();
};
} /* avenger */
