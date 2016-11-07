#include "oi.h"

#include <cmath>

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

namespace sidewinder {

OI::OI(const std::shared_ptr<cpptoml::table> config)
    : flight_sim_joystick_(kFlightSimJoystick) {
  auto c = config->get_table("SIDEWINDER");
  assert(c);
  joystick_dead_zone_ =
      static_cast<float>(*c->get_as<double>("joystick_dead_zone"));
  joystick_expo_coeff_ =
      static_cast<float>(*c->get_as<double>("joystick_expo_coeff"));

  joystick_expo_scale_ =
      1.0 / (joystick_expo_coeff_ * std::pow(1.0 - joystick_dead_zone_, 3) +
             (1.0 - joystick_expo_coeff_) * (1.0 - joystick_dead_zone_));
}

/** Returns flight simulator joystick left stick fowards and backwards (Y-axis)
 * input.
 */
float OI::GetTeleDriveForwardAxis() const {
  float axis = -flight_sim_joystick_.GetRawAxis(kFlightSimLeftYAxis);
  return JoystickExpo(axis);
}

/** Returns flight simulator joystick left stick left and right strafe (X-axis)
 * input.
 */
float OI::GetTeleDriveStrafeAxis() const {
  float axis = flight_sim_joystick_.GetRawAxis(kFlightSimLeftXAxis);
  return JoystickExpo(axis);
}

/** Returns flight simulator joystick CW and CCW azimuth (X-axis) input.
 */
float OI::GetTeleDriveAzimuthAxis() const {
  float axis = flight_sim_joystick_.GetRawAxis(kFlightSimRightXAxis);
  return JoystickExpo(axis);
}

/** Joystick sensitivity transfer function.
 * https://plot.ly/~jhh/11/matplotlib-notebook-import-matplotlibp/
 */
float OI::JoystickExpo(float in) const {
  if (std::fabs(in) < joystick_dead_zone_) {
    return 0.0;
  }
  in = in > 0.0 ? in - joystick_dead_zone_ : in + joystick_dead_zone_;
  return (joystick_expo_coeff_ * std::pow(in, 3) +
          (1 - joystick_expo_coeff_) * in) *
         joystick_expo_scale_;
}
} /* sidewinder */
