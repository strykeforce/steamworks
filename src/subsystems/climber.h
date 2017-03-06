#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace subsystem {

class Climber : public frc::Subsystem {
 public:
  Climber(const std::shared_ptr<cpptoml::table> config);
  Climber& operator=(Climber&) = delete;
  Climber(Climber&) = delete;

  void SetCaptureModeEnabled(bool enabled);
  void StartCapture();
  void StartClimb();
  void Stop();
  bool IsRunning();
  bool IsCaptured();

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  double capture_voltage_ = 4.0;
  double climb_voltage_ = 12.0;
  double capture_current_ = 4.0;
  bool is_running_ = false;

  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
};

} /* subsystem */
} /* steamworks */
