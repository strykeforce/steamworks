#pragma once

#include <WPILib.h>
#include <cpptoml/cpptoml.h>
#include <spdlog/spdlog.h>

#include "robot_map.h"

namespace sidewinder {
namespace talon {
class Settings;
} /* talon */
} /* sidewinder */

namespace steamworks {
namespace subsystem {

class GearLoader : public frc::Subsystem {
 public:
  GearLoader(const std::shared_ptr<cpptoml::table> config);
  GearLoader& operator=(GearLoader&) = delete;
  GearLoader(GearLoader&) = delete;

  void Load();
  void Deploy();
  void StopLoader();

  void SetLoadLimitSwitchNormallyOpen(bool open);
  bool IsLoadLimitSwitchClosed();
  bool IsIntakeLimitSwitchClosed();

  void ClampStage();
  void ClampShut();
  void ClampRelease();
  void ClampStow();

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
  Servo left_servo_{RobotMap::kGearClampLeft};
  Servo right_servo_{RobotMap::kGearClampRight};
  double load_voltage_ = 12.0;
  double deploy_voltage_ = 8.0;

  double left_clamp_stage_ = 0.75;
  double right_clamp_stage_ = 0.32;

  double left_clamp_shut_ = 0.5;
  double right_clamp_shut_ = 0.5;

  double left_clamp_release_ = 0.84;
  double right_clamp_release_ = 0.2;

  double left_clamp_stow_ = 0.45;
  double right_clamp_stow_ = 0.6;

  int pivot_zero_position_ = 825;
  int pivot_up_position_ = 1000;
  int pivot_down_position_ = 10;

  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
};

} /* subsystem */
} /* steamworks */
