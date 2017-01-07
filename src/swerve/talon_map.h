#pragma once

#include "CANTalon.h"

namespace sidewinder {

/** The TalonMap stucture is used by the robot to pass Talon * pointers to the
 * swerve drive.
 */
struct TalonMap {
  ::CANTalon* lf_drive;
  ::CANTalon* lf_azimuth;
  ::CANTalon* rf_drive;
  ::CANTalon* rf_azimuth;
  ::CANTalon* lr_drive;
  ::CANTalon* lr_azimuth;
  ::CANTalon* rr_drive;
  ::CANTalon* rr_azimuth;
};

} /* sidewinder */
