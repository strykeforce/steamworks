#pragma once

#include <AHRS.h>
#include "WPILib.h"
#include "cpptoml/cpptoml.h"

#include "sidewinder/swerve/talon_map.h"

namespace avenger {

class RobotMap {
 public:
  enum Talons {
    kRightFrontAzimuth = 2,
    kRightFrontDrive = 12,
    kLeftFrontAzimuth = 1,
    kLeftFrontDrive = 11,
    kRightRearAzimuth = 3,
    kRightRearDrive = 13,
    kLeftRearAzimuth = 4,
    kLeftRearDrive = 14,
    kTurret = 5,
  };

  // pointers to talons and azimuth encoder zero count
  static sidewinder::swerve::TalonMap* swerve_talons;

  // pointer to navX gyro singleton
  static std::shared_ptr<AHRS> gyro;

  // pointer to turret talon
  static ::CANTalon* turret_talon;

  // instantiate pointers to hardware objects like Talons, etc.
  static void Init(const std::shared_ptr<cpptoml::table> config);
};

// Joysticks
enum Joysticks {
  kFlightSimJoystick = 0,
  kGamePadJoystick,
};

enum FlightSimAxis {
  kFlightSimRightAxis = 0,
  kFlightSimLeftAxis = 2,
};

enum FlightSimButtons {
  kFlightSimLeftButton = 1,
  kFlightSimRightCornerButton,
  kFlightSimResetButton,
  kFlightSimLeftCornerDownButton,
  kFlightSimLeftCornerUpButton,
};

enum JoystickButtons {
  kGameButtonA = 1,
  kGameButtonB,
};

enum TalonProfileSlots {
  kPrimary = 0,
  kSecondary,
};

}  // namespace avenger
