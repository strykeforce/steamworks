#include "oi.h"

#include <cmath>

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "sidewinder/oi/expo.h"

#include "commands/drive.h"
#include "commands/hopper.h"
#include "commands/intake.h"
#include "commands/log.h"
#include "triggers/trim.h"

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
      shoot_close_button_(&gamepad_joystick_, kGamepadFaceXButton),
      trim_up_(trigger::Trim::kUp),
      trim_down_(trigger::Trim::kDown),
      trim_left_(trigger::Trim::kLeft),
      trim_right_(trigger::Trim::kRight) {
  // flight sim reset button is reserved
  reset_button_.WhenPressed(new command::Log("flight simulator reset button"));

  // flight sim left shoulder 3-position button controlls gear loader
  gear_auto_on_button_.WhenPressed(
      new command::Log("flight simulator gear auto on button"));
  gear_auto_off_button_.WhenPressed(
      new command::Log("flight simulator gear auto off button"));

  // flight sim right shoulder 2-position button controls shooter auto mode
  shooter_auto_button_.WhenPressed(
      new command::Log("flight simulator shooter auto button"));

  // gamepad left shoulder stages gear in loader
  gear_stage_button_.WhenPressed(new command::Log("gamepad gear stage button"));

  // gamepad back button reverses gear loader
  gear_stage_reverse_button_.WhenPressed(
      new command::Log("gamepad gear stage reverse button"));

  // gamepad right shoulder turns on fuel intake
  intake_on_button_.WhenPressed(new command::StartIntake());

  // gamepad B button turns off fuel intake
  intake_off_button_.WhenPressed(new command::StopIntake());

  // gamepad Y button reverses fuel intake
  intake_reverse_button_.WhenPressed(new command::ClearIntake());

  // gamepad A button performs feed shot
  shoot_feed_button_.WhenPressed(new command::Log("gamepad feed shot button"));

  // gamepad X button performs close shot
  shoot_close_button_.WhenPressed(
      new command::Log("gamepad close shot button"));

  // gamepad D-pad trims shooter aimpoint
  trim_up_.WhenActive(new command::DriveSquare());
  trim_down_.WhenActive(new command::Log("trim down active"));
  trim_left_.WhenActive(new command::Log("trim left active"));
  trim_left_.WhenActive(new command::ToggleHopper());
  trim_right_.WhenActive(new command::Log("trim right active"));

  // gamepad start button toggles climber
  climber_button_.WhenPressed(new command::Log("gamepad climber button"));
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
  double axis = flight_sim_joystick_.GetRawAxis(kFlightSimRightXAxis);
  return azimuth_expo_(axis);
}
