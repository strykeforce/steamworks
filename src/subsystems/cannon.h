#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

namespace avenger {

class Cannon : public ::Subsystem {
 private:
  const std::shared_ptr<spdlog::logger> logger_;
  std::vector<std::unique_ptr<::Solenoid>> solenoids_;
  double fire_duration_sec_ = 0.0;
  int barrel_ = -1;
  std::unique_ptr<::Notifier> notifier_;
  // TODO: should this be atomic?
  bool firing_ = false;

  void CloseSolenoid();

 public:
  Cannon(const std::shared_ptr<cpptoml::table> config);
  Cannon(const Cannon&) = delete;
  Cannon& operator=(const Cannon&) = delete;
  virtual ~Cannon() = default;

  void Fire();
};
} /* avenger */
