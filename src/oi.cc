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
    : drive_joystick_(kUSB0),
      gamepad_joystick_(kUSB1),
      reset_drive_button_(nullptr),
      fire_cannon_button_(nullptr),
      reset_cannon_button_(nullptr) {
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
  reset_drive_button_.reset(
      new ::JoystickButton(&drive_joystick_, kFlightSimResetButton));
  fire_cannon_button_.reset(
      new ::JoystickButton(&gamepad_joystick_, kGamepadXButton));
  reset_cannon_button_.reset(
      new ::JoystickButton(&gamepad_joystick_, kGamepadSelectButton));

  reset_drive_button_->WhenPressed(new ZeroDriveWheels());
  fire_cannon_button_->WhenPressed(new FireCannon());
  reset_cannon_button_->WhenPressed(new ResetCannon());

  // drive axes
  c = config->get_table("AVENGER");
  assert(c);
  auto joystick_type =
      c->get_as<std::string>("drive_joystick_type").value_or("");
  if (joystick_type == "flight_sim") {
    drive_forward_axis_ = kFlightSimLeftYAxis;
    drive_strafe_axis_ = kFlightSimLeftXAxis;
    drive_azimuth_axis_ = kFlightSimRightXAxis;
  } else if (joystick_type == "gamepad") {
    drive_forward_axis_ = kGamepadLeftYAxis;
    drive_strafe_axis_ = kGamepadLeftXAxis;
    drive_azimuth_axis_ = kGamepadRightXAxis;
  } else {
    throw std::invalid_argument("unrecognized joystick type: " + joystick_type);
  }
}

/** Returns flight simulator joystick left stick fowards and backwards (Y-axis)
 * input.
 */
float OI::GetTeleDriveForwardAxis() const {
  float axis = drive_joystick_.GetRawAxis(drive_forward_axis_);
  return JoystickExpo(axis);
}

/** Returns flight simulator joystick left stick left and right strafe (X-axis)
 * input.
 */
float OI::GetTeleDriveStrafeAxis() const {
  float axis = drive_joystick_.GetRawAxis(drive_strafe_axis_);
  return JoystickExpo(axis);
}

/** Returns flight simulator joystick CW and CCW azimuth (X-axis) input.
 */
float OI::GetTeleDriveAzimuthAxis() const {
  float axis = -drive_joystick_.GetRawAxis(drive_azimuth_axis_);
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
