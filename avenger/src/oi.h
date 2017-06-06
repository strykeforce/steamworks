#pragma once

#include <memory>

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

namespace avenger {

/** Robot OI provides operator inputs.
 * It assumes the flight simulator joysticks and buttons are used for swerve
 * drive operation while gamepad runs the turret and cannon.
 */
class OI {
 private:
  ::Joystick drive_joystick_;
  ::Joystick gamepad_joystick_;

  std::unique_ptr<::JoystickButton> reset_drive_button_;
  std::unique_ptr<::JoystickButton> fire_cannon_button_;
  std::unique_ptr<::JoystickButton> reset_cannon_button_;

  uint32_t drive_forward_axis_, drive_strafe_axis_, drive_azimuth_axis_;

  float joystick_dead_zone_, joystick_expo_coeff_, joystick_expo_scale_;

  float JoystickExpo(const float in) const;

 public:
  OI(const std::shared_ptr<cpptoml::table> config);
  OI(const OI&) = delete;
  OI& operator=(const OI&) = delete;
  virtual ~OI() = default;

  float GetTeleDriveForwardAxis() const;
  float GetTeleDriveStrafeAxis() const;
  float GetTeleDriveAzimuthAxis() const;
  float GetTurretElevationAxis() const;

  // USB Ports for Joysticks
  enum JoystickUSBPorts {
    kUSB0 = 0,
    kUSB1,
    kUSB2,
    kUSB3,
    kUSB4,
    kUSB5,
  };

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

  enum GamepadButtons {
    kGamepadFaceAButton = 1,
    kGamepadFaceBButton,
    kGamepadFaceXButton,
    kGamepadFaceYButton,
    kGamepadLeftShoulderButton,
    kGamepadRightShoulderButton,
    kGamepadBackButton,
    kGamepadStartButton,
    kGamepadLeftStickButton,
    kGamepadRightStickButton,
  };

  enum GamepadAxis {
    kGamepadLeftXAxis = 0,
    kGamepadLeftYAxis = 1,
    kGamepadRightXAxis = 4,
    kGamepadRightYAxis = 5,
  };
};

}  // namespace avenger
