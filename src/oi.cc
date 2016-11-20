#include "oi.h"

#include <cmath>

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

#include "commands/fire_cannon.h"
#include "commands/reset_cannon.h"
#include "commands/zero_drive_wheels.h"

using namespace avenger;

/** Construct and configure Robot operator input.
 */
OI::OI(const std::shared_ptr<cpptoml::table> config)
    : flight_sim_joystick_(kFlightSimJoystick),
      gamepad_joystick_(kGamepadJoystick),
      reset_drive_button_(&flight_sim_joystick_, kFlightSimResetButton),
      fire_cannon_button_(&gamepad_joystick_, kGamepadXButton),
      reset_cannon_button_(&gamepad_joystick_, kGamepadSelectButton) {
  auto c = config->get_table("SIDEWINDER");
  assert(c);
  joystick_dead_zone_ =
      static_cast<float>(*c->get_as<double>("joystick_dead_zone"));
  joystick_expo_coeff_ =
      static_cast<float>(*c->get_as<double>("joystick_expo_coeff"));

  joystick_expo_scale_ =
      1.0 / (joystick_expo_coeff_ * std::pow(1.0 - joystick_dead_zone_, 3) +
             (1.0 - joystick_expo_coeff_) * (1.0 - joystick_dead_zone_));

  // buttons
  reset_drive_button_.WhenPressed(new ZeroDriveWheels());
  fire_cannon_button_.WhenPressed(new FireCannon());
  reset_cannon_button_.WhenPressed(new ResetCannon());
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
  float axis = -flight_sim_joystick_.GetRawAxis(kFlightSimLeftXAxis);
  return JoystickExpo(axis);
}

/** Returns flight simulator joystick CW and CCW azimuth (X-axis) input.
 */
float OI::GetTeleDriveAzimuthAxis() const {
  float axis = -flight_sim_joystick_.GetRawAxis(kFlightSimRightXAxis);
  return JoystickExpo(axis);
}

/** Returns gamepad elevation input.
 */
float OI::GetTurretElevationAxis() const {
  float axis = -gamepad_joystick_.GetRawAxis(kGamepadLeftYAxis);
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
