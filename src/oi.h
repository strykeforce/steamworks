#pragma once

#include <memory>

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

namespace steamworks {

/** Robot OI provides operator inputs.
 * It assumes the flight simulator joysticks and buttons are used for swerve
 * drive operation while gamepad runs the turret and cannon.
 */
class OI {
 private:
  frc::Joystick drive_joystick_;
  frc::Joystick gamepad_joystick_;

 public:
  OI(const std::shared_ptr<cpptoml::table> config);
  OI(const OI&) = delete;
  OI& operator=(const OI&) = delete;
  virtual ~OI() = default;

  float GetTeleDriveForwardAxis() const;
  float GetTeleDriveStrafeAxis() const;
  float GetTeleDriveAzimuthAxis() const;

  // USB Ports for Joysticks
  enum JoystickUSBPorts {
    kUSB0 = 0,
    kUSB1,
    kUSB2,
    kUSB3,
    kUSB4,
    kUSB5,
  };

  // TODO: verify these
  enum FlightSimAxis {
    kFlightSimRightXAxis = 0,
    kFlightSimRightYAxis,
    kFlightSimLeftYAxis,
    kFlightSimTunerAxis,
    kFlightSimLeftXAxis,
  };

  enum FlightSimButtons {
    kFlightSimLeftButton = 1,
    kFlightSimRightCornerButton,
    kFlightSimResetButton,
    kFlightSimLeftCornerDownButton,
    kFlightSimLeftCornerUpButton,
  };

  enum GamepadAxis {
    kGamepadLeftXAxis = 0,
    kGamepadLeftYAxis = 1,
    kGamepadRightXAxis = 4,
    kGamepadRightYAxis = 5,
  };

  enum GamepadButtons {
    kGamepadAButton = 1,
    kGamepadBButton = 2,
    kGamepadXButton = 3,
    kGamepadYButton = 4,
    kGamepadLeftBumperButton = 5,
    kGamepadRightBumperButton = 6,
    kGamepadSelectButton = 7,
    kGamepadStartButton = 8,
  };
};

} /* steamworks */
