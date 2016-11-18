#pragma once

#include <memory>

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

namespace avenger {

/** Robot OI provides swerve drive operator inputs.
 * It assumes the flight simulator joysticks and buttons are used for swerve
 * drive operation.
 */
class OI {
 private:
  // create our joystick objects
  ::Joystick flight_sim_joystick_;
  ::JoystickButton reset_button_;
  float joystick_dead_zone_, joystick_expo_coeff_, joystick_expo_scale_;

  float JoystickExpo(const float in) const;

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

  OI(const std::shared_ptr<cpptoml::table> config);
  virtual ~OI() = default;
  float GetTeleDriveForwardAxis() const;
  float GetTeleDriveStrafeAxis() const;
  float GetTeleDriveAzimuthAxis() const;
};

} /* avenger */