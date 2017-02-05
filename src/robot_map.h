#pragma once

#include "cpptoml/cpptoml.h"

#include "sidewinder/swerve/talon_map.h"

namespace steamworks {

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
    kShooterWheel = 21,
    kShooterElevation = 22,
    kHopper = 31,
    kIntake = 41,
    kGearIntake = 51,
    kGearElevation = 52,
    kClimberMaster = 61,
    kClimberSlave = 62,
  };
  // pointers to talons and azimuth encoder zero count
  static sidewinder::swerve::TalonMap* swerve_talons;

  static ::CANTalon* shooter_wheel_talon;
  static ::CANTalon* shooter_elevation_talon;
  static ::CANTalon* hopper_talon;
  static ::CANTalon* intake_talon;
  static ::CANTalon* gear_intake_talon;
  static ::CANTalon* gear_elevation_talon;
  static ::CANTalon* climber_master_talon;
  static ::CANTalon* climber_slave_talon;

  // instantiate talons and read encoder zero count
  static void Init(const std::shared_ptr<cpptoml::table> config);
};

// Joysticks
enum Joysticks {
  kFlightSimJoystick = 0,  // global.joy1 in old code
  kGamePadJoystick,        // global.joy2
};

// flight sim axis deadzone 0.08
enum FlightSimAxis {
  kFlightSimRightAxis = 0,  // FlightSim_RightX_Axis
  kFlightSimLeftAxis = 2,   // FlightSim_LeftY_Axis
};

enum FlightSimButtons {
  kFlightSimLeftButton = 1,
  kFlightSimRightCornerButton,
  kFlightSimResetButton,
  kFlightSimLeftCornerDownButton,
  kFlightSimLeftCornerUpButton,
};

// Joystick Buttons
enum JoystickButtons {
  kGameButtonA = 1,
  kGameButtonB,  // 2
};

enum TalonProfileSlots {
  kPrimary,
};

} /* steamworks */
