#include "swerve_drive.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

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

  max_voltage_ =
      static_cast<float>(*drive_cfg_data->get_as<double>("max_voltage"));
  logger_->trace("done with constructor");
}

void SwerveDrive::SetEncoderZero(const std::shared_ptr<cpptoml::table> config) {
  // Encoder has 12-bit resolution so mask with 0xFFF. Add 4096 in case current
  // encoder position is less than zero position. We mask delta again for cases
  // where current encoder position was > zero.
  // int abs_pos = (talon->GetPulseWidthPosition() & 0xFFF) + 0xFFF;
  logger_->trace("setting azimuth zero");

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

// helper function for swerve drive calculations.
// https://www.chiefdelphi.com/media/papers/2426
namespace {
const int L = 1;
const int W = 1;
const float R = std::sqrt(L * L + W * W);

struct DriveData {
  float fwd, str, rcw;
  float ws1, ws2, ws3, ws4;
  float wa1, wa2, wa3, wa4;
};

void swerve_calc(DriveData& dd) {
  float a = dd.str - dd.rcw * (L / R);
  float b = dd.str + dd.rcw * (L / R);
  float c = dd.fwd - dd.rcw * (W / R);
  float d = dd.fwd + dd.rcw * (W / R);
  dd.ws1 = std::sqrt(b * b + c * c);
  dd.ws2 = std::sqrt(b * b + d * d);
  dd.ws3 = std::sqrt(a * a + d * d);
  dd.ws4 = std::sqrt(a * a + c * c);
  dd.wa1 = std::atan2(b, c) * 180 / M_PI;
  dd.wa2 = std::atan2(b, d) * 180 / M_PI;
  dd.wa3 = std::atan2(a, d) * 180 / M_PI;
  dd.wa4 = std::atan2(a, c) * 180 / M_PI;

  // normalize wheel speed if needed
  float max = std::max({dd.ws1, dd.ws2, dd.ws3, dd.ws4});
  if (max > 1.0) {
    dd.ws1 /= max;
    dd.ws2 /= max;
    dd.ws3 /= max;
    dd.ws4 /= max;
  }
}
} /* namespace */

/** Drive in swerve drive mode.
 * @param forward command forward/backwards (Y-axis) motion
 * @param strafe command left/right (X-axis) motion
 * @param azimuth command left/right yaw
 */
void SwerveDrive::Drive(float forward, float strafe, float azimuth) {
  DriveData dd = DriveData();
  dd.fwd = forward;
  dd.str = strafe;
  dd.rcw = azimuth;
  swerve_calc(dd);

  // TODO: round? cast? ignore?
  map_->lf_azimuth->Set(std::round(dd.wa1 * 2048 / 180));
  map_->rf_azimuth->Set(std::round(dd.wa2 * 2048 / 180));
  map_->lr_azimuth->Set(std::round(dd.wa3 * 2048 / 180));
  map_->rr_azimuth->Set(std::round(dd.wa4 * 2048 / 180));

  map_->lf_drive->Set(dd.ws1 * max_voltage_);
  map_->rf_drive->Set(dd.ws2 * max_voltage_);
  map_->lr_drive->Set(dd.ws3 * max_voltage_);
  map_->rr_drive->Set(dd.ws4 * max_voltage_);
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
