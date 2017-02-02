#include "oi.h"

#include <cmath>

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

#include "sidewinder/oi/expo.h"
#include "sidewinder/oi/sma.h"

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

size_t get_uint(const std::shared_ptr<cpptoml::table> config,
                std::string param) {
  auto c = config->get_table("SIDEWINDER");
  if (!c) {
    throw std::invalid_argument("SIDEWINDER config missing");
  }

  auto val = c->get_as<long>(param);
  if (!val) {
    throw std::invalid_argument(param + " config missing");
  }
  return static_cast<size_t>(*val);
}

} /* namespace */

/** Construct and configure Robot operator input.
 */
OI::OI(const std::shared_ptr<cpptoml::table> config)
    : flight_sim_joystick_(kFlightSimJoystick),
      reset_button_(&flight_sim_joystick_, kFlightSimResetButton),
      drive_expo_(get_double(config, "drive_dead_zone"),
                  get_double(config, "drive_expo_scale")),
      azimuth_expo_(get_double(config, "azimuth_dead_zone"),
                    get_double(config, "azimuth_expo_scale")),
      drive_sma_(get_uint(config, "drive_sma_period")),
      azimuth_sma_(get_uint(config, "azimuth_sma_period")) {
  // buttons
  // reset_button_.WhenPressed(new ZeroDriveWheels());
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
