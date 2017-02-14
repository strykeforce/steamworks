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
    : logger_(spdlog::get("sidewinder")), map_(tm), swerve_math_(config) {
  assert(tm);

  if (!logger_) {
    logger_ = spdlog::stdout_logger_st("sidewinder");
  }
  logger_->set_level(spdlog::level::info);

  // load sidewinder configuration from common config file
  if (!config) {
    throw std::invalid_argument("config must not be null");
  }

  auto settings = config->get_table("SIDEWINDER");
  if (!settings) {
    throw std::invalid_argument("SIDEWINDER config is missing");
  }

  // configure logging for this class

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
 * @param forward command forward/backwards (Y-axis) throttle, -1.0 - 1.0
 * @param strafe command left/right (X-axis) throttle, -1.0 - 1.0
 * @param azimuth command CW/CCW azimuth throttle, -1.0 - 1.0
 */
void SwerveDrive::Drive(double forward, double strafe, double azimuth) {
  // don't reset wheels to zero in dead zone
  // FIXME: dead zone hard-coded
  if (std::fabs(forward) <= 0.08 && std::fabs(strafe) <= 0.08 &&
      std::fabs(azimuth) < 0.08) {
    map_->rf_drive->StopMotor();
    map_->lf_drive->StopMotor();
    map_->lr_drive->StopMotor();
    map_->rr_drive->StopMotor();
    return;
  }
  DriveData dd = DriveData();
  dd.fwd = forward;
  dd.str = strafe;
  dd.rcw = azimuth;
  swerve_math_(dd, RotationPoint::Center);

  map_->lf_azimuth->Set(std::round(dd.walf * 2048 / 180));
  map_->rf_azimuth->Set(std::round(dd.warf * 2048 / 180));
  map_->lr_azimuth->Set(std::round(dd.walr * 2048 / 180));
  map_->rr_azimuth->Set(std::round(dd.warr * 2048 / 180));

  map_->rf_drive->Set(dd.wsrf * drive_scale_factor_);
  map_->lf_drive->Set(dd.wslf * drive_scale_factor_);
  map_->lr_drive->Set(dd.wslr * drive_scale_factor_);
  map_->rr_drive->Set(dd.wsrr * drive_scale_factor_);

  static int i;
  if (++i == 50) {
    i = 0;
    logger_->trace("warf = {}, walf = {}, walr = {}, warr = {}", dd.warf,
                   dd.walf, dd.walr, dd.warr);
    logger_->trace("wsrf = {}, wslf = {}, wslr = {}, wsrr = {}", dd.wsrf,
                   dd.wslf, dd.wslr, dd.wsrr);
  }
}

/** Rotate around an off-center point, used for pivoting on robot shooter.
 * @param azimuth command CW/CCW azimuth throttle, -1.0 - 1.0
 */
void SwerveDrive::TargetRotation(double azimuth) {
  DriveData dd = DriveData();
  dd.fwd = 0;
  dd.str = 0;
  dd.rcw = azimuth;
  swerve_math_(dd, RotationPoint::Shooter);

  map_->lf_azimuth->Set(std::round(dd.walf * 2048 / 180));
  map_->rf_azimuth->Set(std::round(dd.warf * 2048 / 180));
  map_->lr_azimuth->Set(std::round(dd.walr * 2048 / 180));
  map_->rr_azimuth->Set(std::round(dd.warr * 2048 / 180));

  map_->rf_drive->Set(dd.wsrf * drive_scale_factor_);
  map_->lf_drive->Set(dd.wslf * drive_scale_factor_);
  map_->lr_drive->Set(dd.wslr * drive_scale_factor_);
  map_->rr_drive->Set(dd.wsrr * drive_scale_factor_);
}

/** Drive in crab drive mode.
 * @param speed command forward/backwards (Y-axis) throttle, -1.0 - 1.0
 * @param direction command left/right (X-axis), -1.0 - 1.0
 */
void SwerveDrive::CrabDrive(double speed, double direction) {
  int pos = -std::round(2048 * direction);
  map_->lf_azimuth->Set(pos);
  map_->rf_azimuth->Set(pos);
  map_->lr_azimuth->Set(pos);
  map_->rr_azimuth->Set(pos);

  double setpoint = speed * drive_scale_factor_;
  map_->lf_drive->Set(setpoint);
  map_->rf_drive->Set(setpoint);
  map_->lr_drive->Set(setpoint);
  map_->rr_drive->Set(setpoint);
}

/** Get encoder value of specified drive wheel.
 *  @param wheel select wheel to read
 */
int SwerveDrive::GetPosition(const Wheel wheel) const {
  switch (wheel) {
    case kRightRear:
      return static_cast<int>(map_->rr_drive->GetPosition());
    case kLeftRear:
      return static_cast<int>(map_->lr_drive->GetPosition());
    case kRightFront:
      return static_cast<int>(map_->rf_drive->GetPosition());
    case kLeftFront:
      return static_cast<int>(map_->lf_drive->GetPosition());
  }
  return 0;
}

/** Override the default logger.
 * This logger is expected to have the name "sidewinder".
 */
void SwerveDrive::SetLogger(const std::shared_ptr<spdlog::logger> logger) {
  logger_ = logger;
  logger_->debug("overriding logger");
}
