#pragma once

#include "WPILib.h"

namespace sidewinder {

class RobotMap {
 public:
  enum Talons {
    kLeftFrontDrive = 11,
    kLeftFrontAzimuth = 1,
  };

  static ::CANTalon* lf_drive_talon;
  static ::CANTalon* lf_azimuth_talon;

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

} /* sidewinder */
