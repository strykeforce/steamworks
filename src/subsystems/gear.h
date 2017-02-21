#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "sidewinder/talon/settings.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace subsystem {

class GearLoader : public frc::Subsystem {
 public:
  GearLoader(const std::shared_ptr<cpptoml::table> config);
  GearLoader& operator=(GearLoader&) = delete;
  GearLoader(GearLoader&) = delete;

  void SetLimitSwitchNormallyOpen(bool open);
  void Load();
  void Deploy();
  void StopLoader();

  bool IsIntakeEnabled();
  bool IsLimitSwitchClosed();

  void SetServo(double value);
  double GetServo();

  void SetPivotZeroModeEnabled(bool enabled);
  int GetPivotPosition();
  void SetPivotEncoderZero();
  void PivotUp();
  void PivotDown();

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  std::unique_ptr<sidewinder::talon::Settings> pivot_zero_settings_;
  std::unique_ptr<sidewinder::talon::Settings> pivot_settings_;
  Servo servo_{0};
  double load_voltage_ = 0.0;
  double deploy_voltage_ = 0.0;
};

} /* subsystem */
} /* steamworks */
