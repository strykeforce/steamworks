#include "robot_map.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "sidewinder/grapher/HostToObject.h"
#include "sidewinder/swerve/talon_map.h"

using namespace avenger;

// helper functions to start grapher data collection thread if enabled.
namespace {
inline JerrysGrapher_DeviceBundle jgdb(byte id, ::CANTalon* t) {
  JerrysGrapher_DeviceBundle db{JerrysGrapher_DeviceType::Talon, id, t};
  return db;
}

static std::vector<JerrysGrapher_DeviceBundle> gd;

void initialize_grapher() {
  typedef RobotMap rm;

  gd.push_back(jgdb(rm::kLeftFrontDrive, rm::swerve_talons->lf_drive));
  gd.push_back(jgdb(rm::kLeftFrontAzimuth, rm::swerve_talons->lf_azimuth));

  gd.push_back(jgdb(rm::kRightFrontDrive, rm::swerve_talons->rf_drive));
  gd.push_back(jgdb(rm::kRightFrontAzimuth, rm::swerve_talons->rf_azimuth));

  gd.push_back(jgdb(rm::kLeftRearDrive, rm::swerve_talons->lr_drive));
  gd.push_back(jgdb(rm::kLeftRearAzimuth, rm::swerve_talons->lr_azimuth));

  gd.push_back(jgdb(rm::kRightRearDrive, rm::swerve_talons->rr_drive));
  gd.push_back(jgdb(rm::kRightRearAzimuth, rm::swerve_talons->rr_azimuth));

  for (const auto& d : gd) {
    ::CANTalon* talon = static_cast<::CANTalon*>(d.objectPointer);
    talon->SetStatusFrameRateMs(::CANTalon::StatusFrameRateGeneral, 20);
    talon->SetStatusFrameRateMs(::CANTalon::StatusFrameRateFeedback, 20);
    talon->SetStatusFrameRateMs(::CANTalon::StatusFrameRateQuadEncoder, 20);
    talon->SetStatusFrameRateMs(::CANTalon::StatusFrameRatePulseWidthMeas, 20);
    talon->SetStatusFrameRateMs(::CANTalon::StatusFrameRateAnalogTempVbat, 20);
  }

  JerrysGrapher_StartStatusThread(&gd);
}

} /* namespace */

/** Holds pointers to the 8 swerve drive Talons.  */
sidewinder::swerve::TalonMap* RobotMap::swerve_talons =
    new sidewinder::swerve::TalonMap();

/** Holds pointer to turret arm Talon. */
::CANTalon* RobotMap::turret_talon{nullptr};

std::shared_ptr<AHRS> RobotMap::gyro;

/** Initialize hardware design-specific components.
 * Any run-time configuration should be done in the config file where possible.
 * We allocate these as singletons since there should only be one system-wide
 * reference to each.
 */
void RobotMap::Init(const std::shared_ptr<cpptoml::table> config) {
  gyro = std::make_shared<AHRS>(SPI::Port::kMXP, 200);
  turret_talon = new ::CANTalon(Talons::kTurret);

  swerve_talons->lf_drive = new ::CANTalon(Talons::kLeftFrontDrive);
  swerve_talons->lf_azimuth = new ::CANTalon(Talons::kLeftFrontAzimuth);

  swerve_talons->rf_drive = new ::CANTalon(Talons::kRightFrontDrive);
  swerve_talons->rf_azimuth = new ::CANTalon(Talons::kRightFrontAzimuth);

  swerve_talons->lr_drive = new ::CANTalon(Talons::kLeftRearDrive);
  swerve_talons->lr_azimuth = new ::CANTalon(Talons::kLeftRearAzimuth);

  swerve_talons->rr_drive = new ::CANTalon(Talons::kRightRearDrive);
  swerve_talons->rr_azimuth = new ::CANTalon(Talons::kRightRearAzimuth);

  // start grapher data collection thread if enabled in config file.
  auto c = config->get_table("AVENGER");
  assert(c);
  if (c->get_as<bool>("grapher").value_or(false)) {
    spdlog::get("robot")->warn("initializing grapher");
    initialize_grapher();
  }
}
