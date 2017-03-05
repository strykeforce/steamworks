#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace subsystem {

class Hopper : public frc::Subsystem {
 public:
  Hopper(const std::shared_ptr<cpptoml::table> config);
  Hopper& operator=(Hopper&) = delete;
  Hopper(Hopper&) = delete;

  void Start();
  void Stop();
  bool IsRunning();

  double GetVoltage();
  void SetVoltage(double voltage);

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  double voltage_;
  bool is_running_;
};

} /* subsystem */
} /* steamworks */
