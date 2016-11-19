#include "robot_map.h"

#include "WPILib.h"

#include "swerve/talon_map.h"

using namespace avenger;

/** Holds pointers to the 8 swerve drive Talons.  */
sidewinder::TalonMap* RobotMap::swerve_talons = new sidewinder::TalonMap();

/** Holds pointer to turret arm Talon. */
std::unique_ptr<::CANTalon> RobotMap::turret_talon{nullptr};

/** Initialize hardware design-specific components.
 * Any run-time configuration should be done in the config file where possible.
 * We allocate these as singletons since there should only be one system-wide
 * reference to each.
 */
void RobotMap::Init() {
  turret_talon.reset(new ::CANTalon(Talons::kTurret));

  swerve_talons->lf_drive = new ::CANTalon(Talons::kLeftFrontDrive);
  swerve_talons->lf_azimuth = new ::CANTalon(Talons::kLeftFrontAzimuth);

  swerve_talons->rf_drive = new ::CANTalon(Talons::kRightFrontDrive);
  swerve_talons->rf_azimuth = new ::CANTalon(Talons::kRightFrontAzimuth);

  swerve_talons->lr_drive = new ::CANTalon(Talons::kLeftRearDrive);
  swerve_talons->lr_azimuth = new ::CANTalon(Talons::kLeftRearAzimuth);

  swerve_talons->rr_drive = new ::CANTalon(Talons::kRightRearDrive);
  swerve_talons->rr_azimuth = new ::CANTalon(Talons::kRightRearAzimuth);
}
