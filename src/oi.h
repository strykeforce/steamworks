#pragma once

#include <memory>

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "sidewinder/oi/expo.h"
#include "sidewinder/oi/sma.h"

namespace steamworks {

/** Robot OI provides swerve drive operator inputs.
 * It assumes the flight simulator joysticks and buttons are used for swerve
 * drive operation.
 */
class OI {
 private:
  // create our joystick objects
  sidewinder::oi::Expo drive_expo_, azimuth_expo_;
  frc::Joystick flight_sim_joystick_;
  frc::Joystick gamepad_joystick_;

  // flight sim buttons
  frc::JoystickButton reset_button_;
  frc::JoystickButton gear_auto_on_button_;
  frc::JoystickButton gear_auto_off_button_;
  frc::JoystickButton shooter_auto_button_;

  // gamepad buttons
  frc::JoystickButton gear_stage_button_;
  frc::JoystickButton gear_stage_reverse_button_;
  frc::JoystickButton climber_button_;
  frc::JoystickButton intake_on_button_;
  frc::JoystickButton intake_off_button_;
  frc::JoystickButton intake_reverse_button_;
  frc::JoystickButton shoot_feed_button_;
  frc::JoystickButton shoot_close_button_;

 public:
  enum Joysticks {
    kFlightSimJoystick = 0,
    kGamePadJoystick,
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

  OI(const std::shared_ptr<cpptoml::table> config);
  virtual ~OI() = default;
  double GetTeleDriveForwardAxis();
  double GetTeleDriveStrafeAxis();
  double GetTeleDriveAzimuthAxis();
};

} /* steamworks */
