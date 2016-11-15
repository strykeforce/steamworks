#include "robot_map.h"

#include "WPILib.h"

#include "HostToObject.h"
#include "robot_config.h"
#include "swerve/talon_map.h"

namespace sidewinder {

namespace {
inline JerrysGrapher_DeviceBundle jgdb(byte id, ::CANTalon* t) {
  JerrysGrapher_DeviceBundle db{JerrysGrapher_DeviceType::Talon, id, t};
  return db;
}

void initialize_grapher() {
  typedef RobotMap rm;
  std::vector<JerrysGrapher_DeviceBundle> gd;
  gd.push_back(jgdb(rm::kLeftFrontDrive, rm::swerve_talons->lf_drive));
  gd.push_back(jgdb(rm::kLeftFrontAzimuth, rm::swerve_talons->lf_azimuth));

  gd.push_back(jgdb(rm::kRightFrontDrive, rm::swerve_talons->rf_drive));
  gd.push_back(jgdb(rm::kRightFrontAzimuth, rm::swerve_talons->rf_azimuth));

  gd.push_back(jgdb(rm::kLeftRearDrive, rm::swerve_talons->lr_drive));
  gd.push_back(jgdb(rm::kLeftRearAzimuth, rm::swerve_talons->lr_azimuth));

  gd.push_back(jgdb(rm::kRightRearDrive, rm::swerve_talons->rr_drive));
  gd.push_back(jgdb(rm::kRightRearAzimuth, rm::swerve_talons->rr_azimuth));

  JerrysGrapher_StartStatusThread(&gd);
}

} /* namespace */

/** Holds pointers to the 8 swerve drive Talons.  */
TalonMap* RobotMap::swerve_talons = new TalonMap();

/** Initialize hardware design-specific components.
 * Any run-time configuration should be done in the config file where possible.
 * We allocate these as singletons since there should only be one system-wide
 * reference to each.
 */
void RobotMap::Init() {
  swerve_talons->lf_drive = new ::CANTalon(Talons::kLeftFrontDrive);
  swerve_talons->lf_azimuth = new ::CANTalon(Talons::kLeftFrontAzimuth);

  swerve_talons->rf_drive = new ::CANTalon(Talons::kRightFrontDrive);
  swerve_talons->rf_azimuth = new ::CANTalon(Talons::kRightFrontAzimuth);

  swerve_talons->lr_drive = new ::CANTalon(Talons::kLeftRearDrive);
  swerve_talons->lr_azimuth = new ::CANTalon(Talons::kLeftRearAzimuth);

  swerve_talons->rr_drive = new ::CANTalon(Talons::kRightRearDrive);
  swerve_talons->rr_azimuth = new ::CANTalon(Talons::kRightRearAzimuth);

#ifdef GRAPHER
  initialize_grapher();
#endif
}

} /* sidewinder */
