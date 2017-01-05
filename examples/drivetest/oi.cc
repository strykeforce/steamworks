#include "oi.h"

#include <cmath>

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

#include "commands/zero_drive_wheels.h"

using namespace sidewinder;

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
    : flight_sim_joystick_(kFlightSimJoystick),
      reset_button_(&flight_sim_joystick_, kFlightSimResetButton),
      drive_expo_(get_double(config, "drive_dead_zone"),
                  get_double(config, "drive_drive_expo_scale")),
      azimuth_expo_(get_double(config, "azimuth_dead_zone"),
                    get_double(config, "azimuth_drive_expo_scale")) {
  // buttons
  reset_button_.WhenPressed(new ZeroDriveWheels());
}

/** Returns flight simulator joystick left stick fowards and backwards (Y-axis)
 * input.
 */
float OI::GetTeleDriveForwardAxis() const {
  float axis = flight_sim_joystick_.GetRawAxis(kFlightSimLeftYAxis);
  return drive_expo_(axis);
}

/** Returns flight simulator joystick left stick left and right strafe (X-axis)
 * input.
 */
float OI::GetTeleDriveStrafeAxis() const {
  float axis = flight_sim_joystick_.GetRawAxis(kFlightSimLeftXAxis);
  return drive_expo_(axis);
}

/** Returns flight simulator joystick CW and CCW azimuth (X-axis) input.
 */
float OI::GetTeleDriveAzimuthAxis() const {
  float axis = -flight_sim_joystick_.GetRawAxis(kFlightSimRightXAxis);
  return azimuth_expo_(axis);
}
