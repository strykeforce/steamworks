#include "oi.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

using namespace steamworks;

/** Construct and configure Robot operator input.
 */
OI::OI(const std::shared_ptr<cpptoml::table> config)
    : drive_joystick_(kUSB0), gamepad_joystick_(kUSB1) {
  auto c = config->get_table("STEAMWORKS");
  assert(c);
}

/** Returns flight simulator joystick left stick fowards and backwards (Y-axis)
 * input.
 */
float OI::GetTeleDriveForwardAxis() const {
  float axis = drive_joystick_.GetRawAxis(kFlightSimLeftYAxis);
  return axis;
}

/** Returns flight simulator joystick left stick left and right strafe (X-axis)
 * input.
 */
float OI::GetTeleDriveStrafeAxis() const {
  float axis = drive_joystick_.GetRawAxis(kFlightSimLeftXAxis);
  return axis;
}

/** Returns flight simulator joystick CW and CCW azimuth (X-axis) input.
 */
float OI::GetTeleDriveAzimuthAxis() const {
  float axis = -drive_joystick_.GetRawAxis(kFlightSimRightXAxis);
  return axis;
}
