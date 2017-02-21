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

  bool IsLimitSwitchClosed();

  void ClampOpen();
  void ClampShut();
  void ClampRelease();

  void SetPivotZeroModeEnabled(bool enabled);
  int GetPivotPosition();
  void SetPivotEncoderZero();
  void PivotUp();
  void PivotDown();
  bool IsPivotUp();
  bool IsPivotDown();

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  std::unique_ptr<sidewinder::talon::Settings> pivot_zero_settings_;
  std::unique_ptr<sidewinder::talon::Settings> pivot_settings_;
  Servo servo_{0};
  double load_voltage_ = 12.0;
  double deploy_voltage_ = 8.0;
  double clamp_open_ = 0.7;
  double clamp_shut_ = 0.5;
  double clamp_release_ = 1.0;
  int pivot_up_position_ = 1000;
  int pivot_down_position_ = 10;

  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
};

} /* subsystem */
} /* steamworks */
