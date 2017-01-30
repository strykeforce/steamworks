#pragma once

#include <memory>

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "oi/expo.h"
#include "oi/sma.h"

namespace steamworks {

/** Robot OI provides swerve drive operator inputs.
 * It assumes the flight simulator joysticks and buttons are used for swerve
 * drive operation.
 */
class OI {
 private:
  // create our joystick objects
  frc::Joystick flight_sim_joystick_;
  frc::JoystickButton reset_button_;
  sidewinder::oi::Expo drive_expo_, azimuth_expo_;
  sidewinder::oi::SMA drive_sma_, azimuth_sma_;

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

  enum FlightSimButtons {
    kFlightSimLeftButton = 1,
    kFlightSimRightCornerButton,
    kFlightSimResetButton,
    kFlightSimLeftCornerDownButton,
    kFlightSimLeftCornerUpButton,
  };

  OI(const std::shared_ptr<cpptoml::table> config);
  virtual ~OI() = default;
  double GetTeleDriveForwardAxis();
  double GetTeleDriveStrafeAxis();
  double GetTeleDriveAzimuthAxis();
};

} /* steamworks */
