#include "robot_map.h"

#include "WPILib.h"
#include "spdlog/spdlog.h"

#include "swerve/talon_map.h"

namespace sidewinder {

/** Holds pointers to the 8 swerve drive Talons.  */
TalonMap* RobotMap::swerve_talons = new TalonMap();

/** Initialize hardware design-specific components.
 * Any run-time configuration should be done in the config file where possible.
 * We allocate these as singletons since there should only be one system-wide
 * reference to each.
 */
void RobotMap::Init(const std::shared_ptr<cpptoml::table> config) {
  swerve_talons->lf_drive = new ::CANTalon(Talons::kLeftFrontDrive);
  swerve_talons->lf_azimuth = new ::CANTalon(Talons::kLeftFrontAzimuth);

  swerve_talons->rf_drive = new ::CANTalon(Talons::kRightFrontDrive);
  swerve_talons->rf_azimuth = new ::CANTalon(Talons::kRightFrontAzimuth);

  swerve_talons->lr_drive = new ::CANTalon(Talons::kLeftRearDrive);
  swerve_talons->lr_azimuth = new ::CANTalon(Talons::kLeftRearAzimuth);

  swerve_talons->rr_drive = new ::CANTalon(Talons::kRightRearDrive);
  swerve_talons->rr_azimuth = new ::CANTalon(Talons::kRightRearAzimuth);
}

} /* sidewinder */
