#include "swerve_drive.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

#include "swerve_math.h"
#include "talon/position_talon.h"
#include "talon/voltage_talon.h"
#include "talon_map.h"
#include "tele_drive.h"

namespace sidewinder {

/** Initialize the Sidewinder tele drive command.
 */
void SwerveDrive::InitDefaultCommand() {
  SetDefaultCommand(new TeleDrive(oi_, this));
}

/** Initialize the Sidewinder SwerveDrive.
 * @param config cpptoml SIDEWINDER table
 * @param tm talon map initialized with pointers to drive talons
 * @oi sidewinder-specific operator input
 */
SwerveDrive::SwerveDrive(const std::shared_ptr<cpptoml::table> config,
                         const TalonMap* tm, const OI* oi)
    : Subsystem("SwerveDrive"),
      logger_(spdlog::stdout_color_st(GetName())),
      map_(tm),
      oi_(oi) {
  logger_->set_level(spdlog::level::trace);
  logger_->trace("starting constructor");

  logger_->trace("configuring azimuth talons in position mode");
  auto azimuth_cfg_data = config->get_table("AZIMUTH");
  assert(azimuth_cfg_data);
  auto azimuth_talon_cfg = talon::PositionTalon(azimuth_cfg_data);
  logger_->debug("dumping azimuth talon configuration");
  azimuth_talon_cfg.LogConfig(logger_);

  azimuth_talon_cfg.Configure(map_->lf_azimuth);
  azimuth_talon_cfg.SetMode(map_->lf_azimuth);
  azimuth_talon_cfg.Configure(map_->rf_azimuth);
  azimuth_talon_cfg.SetMode(map_->rf_azimuth);
  azimuth_talon_cfg.Configure(map_->lr_azimuth);
  azimuth_talon_cfg.SetMode(map_->lr_azimuth);
  azimuth_talon_cfg.Configure(map_->rr_azimuth);
  azimuth_talon_cfg.SetMode(map_->rr_azimuth);

  SetEncoderZero(azimuth_cfg_data);

  logger_->trace("configuring drive talons in voltage mode");
  auto drive_cfg_data = config->get_table("DRIVE");
  assert(drive_cfg_data);
  auto drive_talon_cfg = talon::VoltageTalon(drive_cfg_data);
  logger_->debug("dumping drive talon configuration");
  drive_talon_cfg.LogConfig(logger_);

  drive_talon_cfg.Configure(map_->lf_drive);
  drive_talon_cfg.SetMode(map_->lf_drive);
  drive_talon_cfg.Configure(map_->rf_drive);
  drive_talon_cfg.SetMode(map_->rf_drive);
  drive_talon_cfg.Configure(map_->lr_drive);
  drive_talon_cfg.SetMode(map_->lr_drive);
  drive_talon_cfg.Configure(map_->rr_drive);
  drive_talon_cfg.SetMode(map_->rr_drive);

  // this is how hard we will allow driving in voltage mode.
  max_voltage_ =
      static_cast<float>(*drive_cfg_data->get_as<double>("max_voltage"));
  logger_->trace("done with constructor");
}

/** Set the four azimuth quad encoders to match the current absolute offset
 * from zero.
 */
void SwerveDrive::SetEncoderZero(const std::shared_ptr<cpptoml::table> config) {
  logger_->trace("setting azimuth zero");

  // Take lower 12-bits of current absolute encoder position (0-4096) and offset
  // by our recorded zero position. Repeat for all four wheels.
  auto pos = (map_->lf_azimuth->GetPulseWidthPosition() & 0xFFF) -
             *config->get_as<int>("lf_zero");
  map_->lf_azimuth->SetPosition(pos);
  logger_->debug("set left front azimuth position = {}", pos);

  pos = (map_->rf_azimuth->GetPulseWidthPosition() & 0xFFF) -
        *config->get_as<int>("rf_zero");
  map_->rf_azimuth->SetPosition(pos);
  logger_->debug("set right front azimuth position = {}", pos);

  pos = (map_->lr_azimuth->GetPulseWidthPosition() & 0xFFF) -
        *config->get_as<int>("lr_zero");
  map_->lr_azimuth->SetPosition(pos);
  logger_->debug("set left rear azimuth position = {}", pos);

  pos = (map_->rr_azimuth->GetPulseWidthPosition() & 0xFFF) -
        *config->get_as<int>("rr_zero");
  map_->rr_azimuth->SetPosition(pos);
  logger_->debug("set right rear azimuth position = {}", pos);
}

/** Move all wheels to their home position.
 */
void SwerveDrive::ZeroAzimuth() {
  map_->lf_azimuth->Set(0.0);
  map_->rf_azimuth->Set(0.0);
  map_->lr_azimuth->Set(0.0);
  map_->rr_azimuth->Set(0.0);
}

/** Drive in swerve drive mode.
 * @param forward command forward/backwards (Y-axis) motion
 * @param strafe command left/right (X-axis) motion
 * @param azimuth command left/right yaw
 */
void SwerveDrive::Drive(float forward, float strafe, float azimuth) {
  // don't reset wheels to zero in dead zone
  if (std::fabs(forward) <= 0.08 && std::fabs(strafe) <= 0.08 &&
      std::fabs(azimuth) < 0.08) {
    map_->rf_drive->Set(0.0);
    map_->lf_drive->Set(0.0);
    map_->lr_drive->Set(0.0);
    map_->rr_drive->Set(0.0);
    return;
  }
  DriveData dd = DriveData();
  dd.fwd = forward;
  dd.str = strafe;
  dd.rcw = azimuth;
  swerve_math_.Calc(dd);

  // TODO: round? cast? ignore?
  map_->lf_azimuth->Set(std::round(dd.walf * 2048 / 180));
  map_->rf_azimuth->Set(std::round(dd.warf * 2048 / 180));
  map_->lr_azimuth->Set(std::round(dd.walr * 2048 / 180));
  map_->rr_azimuth->Set(std::round(dd.warr * 2048 / 180));

  map_->rf_drive->Set(dd.wsrf * max_voltage_);
  map_->lf_drive->Set(dd.wslf * max_voltage_);
  map_->lr_drive->Set(dd.wslr * max_voltage_);
  map_->rr_drive->Set(dd.wsrr * max_voltage_);
  static int i;
  if (++i == 15) {
    logger_->debug("warf = {}, walf = {}, walr = {}, warr = {}", dd.warf,
                   dd.walf, dd.walr, dd.warr);
    logger_->debug("wsrf = {}, wslf = {}, wslr = {}, wsrr = {}", dd.wsrf,
                   dd.wslf, dd.wslr, dd.wsrr);
    i = 0;
  }
}

/** Drive in crab drive mode.
 * @param forward command forward/backwards (Y-axis) motion
 * @param strafe command left/right (X-axis) motion
 */
void SwerveDrive::CrabDrive(float forward, float strafe) {
  // int pos = std::round(2048 * (strafe < 0 ? 2 + strafe : strafe));
  int pos = -std::round(2048 * strafe);
  // float pos = 0.5 * strafe;
  map_->lf_azimuth->Set(pos);
  map_->rf_azimuth->Set(pos);
  map_->lr_azimuth->Set(pos);
  map_->rr_azimuth->Set(pos);

  float volts = forward * max_voltage_;
  map_->lf_drive->Set(volts);
  map_->rf_drive->Set(volts);
  map_->lr_drive->Set(volts);
  map_->rr_drive->Set(volts);
  static int i;
  if (++i == 15) {
    logger_->debug("crab driving forward = {}, strafe = {}", forward, strafe);
    logger_->debug("crab driving pos = {}, volts = {}", pos, volts);
    i = 0;
  }
}

} /* sidewinder */
