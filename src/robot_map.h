#pragma once

#include "WPILib.h"

#include "swerve/talon_map.h"

namespace avenger {

class RobotMap {
 public:
  // FIXME: need to sort out left/right madness
  // device ids for swerve drive talons
  // enum Talons {
  //   kRightFrontAzimuth = 1,
  //   kRightFrontDrive = 11,
  //   kLeftFrontAzimuth = 2,
  //   kLeftFrontDrive = 12,
  //   kRightRearAzimuth = 4,
  //   kRightRearDrive = 14,
  //   kLeftRearAzimuth = 3,
  //   kLeftRearDrive = 13,
  // };

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
  static sidewinder::TalonMap* swerve_talons;

  // pointer to turret talon
  static std::unique_ptr<::CANTalon> turret_talon;

  // instantiate talons and read encoder zero count
  static void Init();
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

} /* avenger */
