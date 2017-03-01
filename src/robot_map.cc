#include "robot_map.h"

#include <CANTalon.h>
#include <sidewinder/grapher/HostToObject.h>
#include <sidewinder/swerve/talon_map.h>
#include <spdlog/spdlog.h>

using namespace steamworks;
using namespace sidewinder;

// helper functions to start grapher data collection thread if enabled.
namespace {
inline JerrysGrapher_DeviceBundle jgdb(byte id, ::CANTalon* t) {
  JerrysGrapher_DeviceBundle db{JerrysGrapher_DeviceType::Talon, id, t};
  return db;
}

static std::vector<JerrysGrapher_DeviceBundle> gd;

void initialize_grapher() {
#ifndef NDEBUG
  typedef RobotMap rm;

  gd.push_back(jgdb(rm::kLeftFrontDrive, rm::swerve_talons->lf_drive));
  gd.push_back(jgdb(rm::kLeftFrontAzimuth, rm::swerve_talons->lf_azimuth));

  gd.push_back(jgdb(rm::kRightFrontDrive, rm::swerve_talons->rf_drive));
  gd.push_back(jgdb(rm::kRightFrontAzimuth, rm::swerve_talons->rf_azimuth));

  gd.push_back(jgdb(rm::kLeftRearDrive, rm::swerve_talons->lr_drive));
  gd.push_back(jgdb(rm::kLeftRearAzimuth, rm::swerve_talons->lr_azimuth));

  gd.push_back(jgdb(rm::kRightRearDrive, rm::swerve_talons->rr_drive));
  gd.push_back(jgdb(rm::kRightRearAzimuth, rm::swerve_talons->rr_azimuth));

  gd.push_back(jgdb(rm::kHopper, rm::hopper_talon));
  gd.push_back(jgdb(rm::kIntake, rm::intake_talon));

  gd.push_back(jgdb(rm::kShooterWheel, rm::shooter_wheel_talon));
  gd.push_back(jgdb(rm::kShooterElevation, rm::shooter_elevation_talon));

  gd.push_back(jgdb(rm::kClimberMaster, rm::climber_master_talon));
  gd.push_back(jgdb(rm::kClimberSlave, rm::climber_slave_talon));

  gd.push_back(jgdb(rm::kGearIntake, rm::gear_intake_talon));
  gd.push_back(jgdb(rm::kGearPivot, rm::gear_pivot_talon));

  for (const auto& d : gd) {
    ::CANTalon* talon = static_cast<::CANTalon*>(d.objectPointer);
    talon->SetStatusFrameRateMs(::CANTalon::StatusFrameRateGeneral, 20);
    talon->SetStatusFrameRateMs(::CANTalon::StatusFrameRateFeedback, 20);
    talon->SetStatusFrameRateMs(::CANTalon::StatusFrameRateQuadEncoder, 20);
    talon->SetStatusFrameRateMs(::CANTalon::StatusFrameRatePulseWidthMeas, 20);
    talon->SetStatusFrameRateMs(::CANTalon::StatusFrameRateAnalogTempVbat, 20);
  }

  JerrysGrapher_StartStatusThread(&gd);
#endif
}

} /* namespace */

/** Holds pointers to the 8 swerve drive Talons.  */
swerve::TalonMap* RobotMap::swerve_talons = new swerve::TalonMap();

::CANTalon* RobotMap::shooter_wheel_talon{nullptr};
::CANTalon* RobotMap::shooter_elevation_talon{nullptr};
::CANTalon* RobotMap::hopper_talon{nullptr};
::CANTalon* RobotMap::intake_talon{nullptr};
::CANTalon* RobotMap::gear_intake_talon{nullptr};
::CANTalon* RobotMap::gear_pivot_talon{nullptr};
::CANTalon* RobotMap::climber_master_talon{nullptr};
::CANTalon* RobotMap::climber_slave_talon{nullptr};

std::shared_ptr<AHRS> RobotMap::gyro;

/** Initialize hardware design-specific components.
 * Any run-time configuration should be done in the config file where possible.
 * We allocate these as singletons since there should only be one system-wide
 * reference to each.
 */
void RobotMap::Init(const std::shared_ptr<cpptoml::table> config) {
  gyro = std::make_shared<AHRS>(SPI::Port::kMXP);

  shooter_wheel_talon = new ::CANTalon(Talons::kShooterWheel);
  shooter_elevation_talon = new ::CANTalon(Talons::kShooterElevation);

  hopper_talon = new ::CANTalon(Talons::kHopper);

  intake_talon = new ::CANTalon(Talons::kIntake);

  gear_intake_talon = new ::CANTalon(Talons::kGearIntake);
  gear_pivot_talon = new ::CANTalon(Talons::kGearPivot);

  climber_master_talon = new ::CANTalon(Talons::kClimberMaster);
  climber_slave_talon = new ::CANTalon(Talons::kClimberSlave);

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
    initialize_grapher();
  }
}
