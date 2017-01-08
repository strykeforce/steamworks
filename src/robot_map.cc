#include "robot_map.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

// #include "HostToObject.h"
#include "swerve/talon_map.h"

using namespace steamworks;

/** Holds pointers to the 8 swerve drive Talons.  */
sidewinder::TalonMap* RobotMap::swerve_talons = new sidewinder::TalonMap();

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

  // start grapher data collection thread if enabled in config file.
  auto c = config->get_table("STEAMWORKS");
  assert(c);
  if (c->get_as<bool>("grapher").value_or(false)) {
    spdlog::get("robot")->warn("initializing grapher");
    // initialize_grapher();
  }
}