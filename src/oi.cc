#include "oi.h"

#include <cmath>

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "sidewinder/oi/expo.h"

#include "commands/log.h"

using namespace steamworks;

// constructor helpers
namespace {
double get_double(const std::shared_ptr<cpptoml::table> config,
                  std::string param) {
  auto c = config->get_table("SIDEWINDER");
  if (!c) {
    throw std::invalid_argument("SIDEWINDER config missing");
  }

  auto val = c->get_as<double>(param);
  if (!val) {
    throw std::invalid_argument(param + " config missing");
  }
  return *val;
}

} /* namespace */

/** Construct and configure Robot operator input.
 */
OI::OI(const std::shared_ptr<cpptoml::table> config)
    : drive_expo_(get_double(config, "drive_dead_zone"),
                  get_double(config, "drive_expo_scale")),
      azimuth_expo_(get_double(config, "azimuth_dead_zone"),
                    get_double(config, "azimuth_expo_scale")),
      flight_sim_joystick_(kFlightSimJoystick),
      gamepad_joystick_(kGamePadJoystick),
      reset_button_(&flight_sim_joystick_, kFlightSimResetButton),
      gear_auto_on_button_(&flight_sim_joystick_,
                           kFlightSimLeftCornerDownButton),
      gear_auto_off_button_(&flight_sim_joystick_,
                            kFlightSimLeftCornerUpButton),
      shooter_auto_button_(&flight_sim_joystick_, kFlightSimRightCornerButton),
      gear_stage_button_(&gamepad_joystick_, kGamepadLeftShoulderButton),
      gear_stage_reverse_button_(&gamepad_joystick_, kGamepadBackButton),
      climber_button_(&gamepad_joystick_, kGamepadStartButton),
      intake_on_button_(&gamepad_joystick_, kGamepadRightShoulderButton),
      intake_off_button_(&gamepad_joystick_, kGamepadFaceBButton),
      intake_reverse_button_(&gamepad_joystick_, kGamepadFaceYButton),
      shoot_feed_button_(&gamepad_joystick_, kGamepadFaceAButton),
      shoot_close_button_(&gamepad_joystick_, kGamepadFaceXButton) {
  // buttons
  reset_button_.WhenPressed(new command::Log("flight simulator reset button"));
  gear_auto_on_button_.WhenPressed(
      new command::Log("flight simulator gear auto on button"));
  gear_auto_off_button_.WhenPressed(
      new command::Log("flight simulator gear auto off button"));
  shooter_auto_button_.WhenPressed(
      new command::Log("flight simulator shooter auto button"));

  gear_stage_button_.WhenPressed(new command::Log("gamepad gear stage button"));
  gear_stage_reverse_button_.WhenPressed(
      new command::Log("gamepad gear stage reverse button"));
  climber_button_.WhenPressed(new command::Log("gamepad climber button"));
  intake_on_button_.WhenPressed(new command::Log("gamepad intake on button"));
  intake_off_button_.WhenPressed(new command::Log("gamepad intake off button"));
  intake_reverse_button_.WhenPressed(
      new command::Log("gamepad intake reverse button"));
  shoot_feed_button_.WhenPressed(new command::Log("gamepad feed shot button"));
  shoot_close_button_.WhenPressed(
      new command::Log("gamepad close shot button"));
}

/** Returns flight simulator joystick left stick fowards and backwards (Y-axis)
 * input.
 */
double OI::GetTeleDriveForwardAxis() {
  double axis = -flight_sim_joystick_.GetRawAxis(kFlightSimLeftYAxis);
  return drive_expo_(axis);
}

/** Returns flight simulator joystick left stick left and right strafe (X-axis)
 * input.
 */
double OI::GetTeleDriveStrafeAxis() {
  double axis = flight_sim_joystick_.GetRawAxis(kFlightSimLeftXAxis);
  return drive_expo_(axis);
}

/** Returns flight simulator joystick CW and CCW azimuth (X-axis) input.
 */
double OI::GetTeleDriveAzimuthAxis() {
  double axis = -flight_sim_joystick_.GetRawAxis(kFlightSimRightXAxis);
  return azimuth_expo_(axis);
}
