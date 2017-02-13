#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace subsystem {

class Intake : public frc::Subsystem {
 public:
  Intake(const std::shared_ptr<cpptoml::table> config);
  Intake& operator=(Intake&) = delete;
  Intake(Intake&) = delete;

  void Start();
  void Reverse();
  void Stop();
  int GetEncoderVelocity();

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  double voltage_;
};

} /* subsystem */
} /* steamworks */
