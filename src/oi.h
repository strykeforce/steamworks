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
  ::Joystick flight_sim_joystick_;
  ::Joystick gamepad_joystick_;

  ::JoystickButton reset_drive_button_;
  ::JoystickButton fire_cannon_button_;
  ::JoystickButton reset_cannon_button_;

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

  enum Joysticks {
    kFlightSimJoystick = 0,
    kGamepadJoystick,
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

} /* avenger */
