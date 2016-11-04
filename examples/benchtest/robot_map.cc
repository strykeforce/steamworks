#include "robot_map.h"

#include "WPILib.h"

namespace sidewinder {

::CANTalon* RobotMap::lf_drive_talon = nullptr;
::CANTalon* RobotMap::lf_azimuth_talon = nullptr;

/** Initialize hardware design-specific components.
 * Any run-time configuration should be done in the config file where possible.
 * We allocate these as singletons since there should only be one system-wide
 * reference to each.
 */
void RobotMap::Init() {
  lf_drive_talon = new ::CANTalon(Talons::kLeftFrontDrive);
  lf_azimuth_talon = new ::CANTalon(Talons::kLeftFrontAzimuth);
}

} /* sidewinder */
