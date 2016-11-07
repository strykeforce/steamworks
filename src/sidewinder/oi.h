#pragma once

#include <memory>

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

namespace sidewinder {

/** Sidewinder OI provides swerve drive operator inputs.
 * It should be subclassed by robots that have other inputs.
 */
class OI {
 private:
  // create our joystick objects
  ::Joystick flight_sim_joystick_;
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

  OI(const std::shared_ptr<cpptoml::table> config);
  virtual ~OI() = default;
  float GetTeleDriveForwardAxis() const;
  float GetTeleDriveStrafeAxis() const;
  float GetTeleDriveAzimuthAxis() const;
};

} /* sidewinder */
