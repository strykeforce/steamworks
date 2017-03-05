#include "oi.h"

#include <cmath>

#include "commands/commands.h"
#include "robot_map.h"

using namespace steamworks;
using namespace steamworks::command;

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
      gear_load_button_(),
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
  AssignFlightSimButtons();
  AssignGamepadButtons();
  AssignSmartDashboardButtons();
}

/**
 * Returns flight simulator joystick left stick fowards and backwards (Y-axis)
 * input.
 */
double OI::GetTeleDriveForwardAxis() {
  double axis = flight_sim_joystick_.GetRawAxis(kFlightSimLeftYAxis);
  return drive_expo_(axis);
}

/**
 * Returns flight simulator joystick left stick left and right strafe (X-axis)
 * input.
 */
double OI::GetTeleDriveStrafeAxis() {
  double axis = -flight_sim_joystick_.GetRawAxis(kFlightSimLeftXAxis);
  return drive_expo_(axis);
}

/**
 * Returns flight simulator joystick CW and CCW azimuth (X-axis) input.
 */
double OI::GetTeleDriveAzimuthAxis() {
  double axis = flight_sim_joystick_.GetRawAxis(kFlightSimRightXAxis);
  return azimuth_expo_(axis);
}

/**
 * Returns the Auton Mode switch setting.
 */
unsigned OI::GetAutonMode() {
  uint8_t val = 0;

  // iterate unsigned or int backwards to zero, i starts at kAutonSwitchMSB - 1
  for (int i = RobotMap::kAutonSwitchMSB; i-- > 0;) {
    frc::DigitalInput di{i};
    val <<= 1;
    val = (val & 0xFE) | (di.Get() ? 0 : 1);
  }
  return static_cast<unsigned>(val);
}

/**
 * AssignFlightSimButtons hooks up flight simulator controller buttons to
 * commands.
 */
void OI::AssignFlightSimButtons() {
  // flight sim reset button is reserved
  reset_button_.WhenPressed(new Log("flight simulator reset button"));

  // flight sim left shoulder 3-position button controlls gear loader
  gear_auto_off_button_.WhenPressed(
      new Log("flight simulator gear auto off button"));
  gear_auto_on_button_.WhenPressed(new gear::ReleaseGear());

  // flight sim right shoulder 2-position button controls shooter auto mode
  shooter_auto_button_.WhenPressed(
      new Log("flight simulator shooter auto button"));
}

/**
 * AssignGamepadButtons hooks up gamepad controller buttons to commands.
 */
void OI::AssignGamepadButtons() {
  // gamepad left trigger starts gear loader
  gear_load_button_.WhenActive(new gear::LoadGear());

  // gamepad left shoulder stages gear in loader
  gear_stage_button_.WhenPressed(new gear::StageGear());

  // gamepad back button reverses gear loader
  gear_stage_reverse_button_.WhenPressed(
      new Log("gamepad gear stage reverse button"));
  gear_stage_reverse_button_.WhenPressed(new ToggleHopper());

  // gamepad right shoulder turns on fuel intake
  intake_on_button_.WhenPressed(new StartIntake());

  // gamepad B button turns off fuel intake
  intake_off_button_.WhenPressed(new StopIntake());

  // gamepad Y button reverses fuel intake
  intake_reverse_button_.WhenPressed(new ClearIntake());

  // gamepad A button performs feed shot
  shoot_feed_button_.WhenPressed(new Log("gamepad feed shot button"));

  // gamepad X button performs close shot
  shoot_close_button_.WhenPressed(new Log("gamepad close shot button"));

  // gamepad D-pad trims shooter aimpoint
  trim_up_.WhenActive(new ToggleHopper());
  trim_down_.WhenActive(new Log("trim down active"));
  trim_left_.WhenActive(new Log("trim left active"));
  trim_right_.WhenActive(new Log("trim right active"));

  // gamepad start button toggles climber
  climber_button_.WhenPressed(new CaptureRope());
  climber_button_.WhenReleased(new StopClimb());
}

/**
 * AssignSmartDashboardButtons hooks up smart dashboard buttons to commands.
 */
void OI::AssignSmartDashboardButtons() {
  SmartDashboard::PutData("Zero Wheels", new ZeroWheelAzimuth());
  SmartDashboard::PutData("Position Azimuth", new drive::PositionAzimuth());

  SmartDashboard::PutData("Write Azimuth Cal", new WriteAzimuthCalibration());
  SmartDashboard::PutData("Drive Zero", new drive::Zero());
  SmartDashboard::PutData("Zero Gyro", new ZeroGyroYaw());

  SmartDashboard::PutData("Zero Pivot", new gear::ZeroPivot());

  SmartDashboard::PutData("Default Elevation", new SetShooterElevation(1000));
  SmartDashboard::PutData("Increment Elevation",
                          new IncrementShooterElevation());
  SmartDashboard::PutData("Decrement Elevation",
                          new DecrementShooterElevation());

  SmartDashboard::PutData("Start Wheel", new StartShooterWheel());
  SmartDashboard::PutData("Stop Wheel", new StopShooterWheel());

  SmartDashboard::PutData("Increment Speed", new IncrementShooterSpeed());
  SmartDashboard::PutData("Decrement Speed", new DecrementShooterSpeed());

  SmartDashboard::PutData("Gear LED On", new deadeye::GearLED(true));
  SmartDashboard::PutData("Gear LED Off", new deadeye::GearLED(false));
  SmartDashboard::PutData("Shooter LED On", new deadeye::ShooterLED(true));
  SmartDashboard::PutData("Shooter LED Off", new deadeye::ShooterLED(false));
}
