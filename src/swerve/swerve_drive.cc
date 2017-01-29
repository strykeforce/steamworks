#include "swerve_drive.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"

#include "swerve_math.h"
#include "talon/settings.h"
#include "talon_map.h"

using namespace sidewinder::swerve;

/** Initialize the Sidewinder SwerveDrive.
 * @param config cpptoml SIDEWINDER table
 * @param tm talon map initialized with pointers to drive talons
 */
SwerveDrive::SwerveDrive(const std::shared_ptr<cpptoml::table> config,
                         const TalonMap* tm)
    : logger_(spdlog::stdout_color_st("SwerveDrive")), map_(tm) {
  assert(tm);

  // load sidewinder configuration from common config file
  if (!config) {
    throw std::invalid_argument("config must not be null");
  }

  auto settings = config->get_table("SIDEWINDER");
  if (!settings) {
    throw std::invalid_argument("SIDEWINDER config is missing");
  }

  // configure logging for this class
  logger_->set_level(spdlog::level::debug);
  logger_->trace("starting constructor");

  // get swerve drive configuration from sidewinder settings
  logger_->trace("configuring azimuth talons in position mode");
  auto swerve_settings = settings->get_table("SWERVE");
  if (!swerve_settings) {
    throw std::invalid_argument("SIDEWINDER SWERVE config is missing");
  }

  // load azimuth Talon settings from swerve config and initialize azimuth
  // Talons
  auto azimuth_settings = talon::Settings::Create(swerve_settings, "azimuth");
  logger_->debug("dumping azimuth talon configuration");
  azimuth_settings->LogConfig(logger_);

  azimuth_settings->Initialize(map_->lf_azimuth);
  azimuth_settings->Initialize(map_->rf_azimuth);
  azimuth_settings->Initialize(map_->lr_azimuth);
  azimuth_settings->Initialize(map_->rr_azimuth);

  // zero out azimuth encoders based on config values in swerve_settings
  SetEncoderZero(swerve_settings);

  // load drive Talon settings from swerve config and initialize drive Talons
  logger_->trace("configuring drive talons in voltage mode");
  auto drive_settings =
      talon::Settings::Create(swerve_settings, "drive_voltage");
  logger_->debug("dumping drive talon configuration");
  drive_settings->LogConfig(logger_);

  drive_settings->Initialize(map_->lf_drive);
  drive_settings->Initialize(map_->rf_drive);
  drive_settings->Initialize(map_->lr_drive);
  drive_settings->Initialize(map_->rr_drive);

  // max output to CANTalon.Set().
  auto drive_scale_factor = settings->get_as<double>("drive_scale_factor");
  if (!drive_scale_factor) {
    throw std::invalid_argument(
        "SIDEWINDER drive_scale_factor setting is missing");
  }
  drive_scale_factor_ = *drive_scale_factor;
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
void SwerveDrive::Drive(double forward, double strafe, double azimuth) {
  // don't reset wheels to zero in dead zone
  // FIXME: dead zone hard-coded
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
  swerve_math_(dd);

  map_->lf_azimuth->Set(std::round(dd.walf * 2048 / 180));
  map_->rf_azimuth->Set(std::round(dd.warf * 2048 / 180));
  map_->lr_azimuth->Set(std::round(dd.walr * 2048 / 180));
  map_->rr_azimuth->Set(std::round(dd.warr * 2048 / 180));

  map_->rf_drive->Set(dd.wsrf * drive_scale_factor_);
  map_->lf_drive->Set(dd.wslf * drive_scale_factor_);
  map_->lr_drive->Set(dd.wslr * drive_scale_factor_);
  map_->rr_drive->Set(dd.wsrr * drive_scale_factor_);

  static int i;
  if (++i == 15) {
    logger_->trace("warf = {}, walf = {}, walr = {}, warr = {}", dd.warf,
                   dd.walf, dd.walr, dd.warr);
    logger_->trace("wsrf = {}, wslf = {}, wslr = {}, wsrr = {}", dd.wsrf,
                   dd.wslf, dd.wslr, dd.wsrr);
    i = 0;
  }
}

/** Drive in crab drive mode.
 * @param forward command forward/backwards (Y-axis) motion
 * @param strafe command left/right (X-axis) motion
 */
void SwerveDrive::CrabDrive(double forward, double strafe) {
  // int pos = std::round(2048 * (strafe < 0 ? 2 + strafe : strafe));
  int pos = -std::round(2048 * strafe);
  // double pos = 0.5 * strafe;
  map_->lf_azimuth->Set(pos);
  map_->rf_azimuth->Set(pos);
  map_->lr_azimuth->Set(pos);
  map_->rr_azimuth->Set(pos);

  double volts = forward * drive_scale_factor_;
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
