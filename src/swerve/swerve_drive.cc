#include "swerve_drive.h"

#include <ctime>
#include <fstream>

#include "talon/settings.h"

using namespace sidewinder::swerve;

/**
 * The designated constructor for SwerveDrive.
 *
 * It implements frc::Subsystem so it can be used by subclassing in a
 * command-based robot.
 *
 * @param name the name of the Subsystem
 * @param config gets settings from the SIDEWINDER table within
 * @param map pointer to stucture containing Talon objects
 * @param gyro the navX gyro shared by the robot
 */
SwerveDrive::SwerveDrive(const std::string& name,
                         const std::shared_ptr<cpptoml::table> config,
                         const TalonMap* map, std::shared_ptr<AHRS> gyro)
    : frc::Subsystem(name),
      logger_(spdlog::get("sidewinder")),
      map_(map),
      ahrs_(gyro),
      swerve_math_(config) {
  // FIXME: logger_ crashes if no sidewinder log
  assert(map_);

  if (!logger_) {
    logger_ = spdlog::stdout_logger_st("sidewinder");
  }

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

  // zero out azimuth encoders based on stored config values
  ReadAzimuthCalibration();

  // load drive Talon settings from swerve config and initialize drive
  // Talons
  logger_->trace("configuring drive talons in voltage mode");
  auto drive_settings = talon::Settings::Create(swerve_settings, "drive");
  logger_->debug("dumping drive talon configuration");
  drive_settings->LogConfig(logger_);

  drive_settings->Initialize(map_->lf_drive);
  drive_settings->Initialize(map_->rf_drive);
  drive_settings->Initialize(map_->lr_drive);
  drive_settings->Initialize(map_->rr_drive);

  // max output to CANTalon.Set().
  drive_scale_factor_ = drive_settings->GetSetpointMax();

  auto dead_zone_ = settings->get_as<double>("drive_dead_zone");
  if (!dead_zone_) {
    throw std::invalid_argument(
        "SIDEWINDER drive_dead_zone setting is missing");
  }
  dead_zone = *dead_zone_;
  logger_->trace("done with constructor");
}

/**
 * Instantiate a SwerveDrive with default name.
 */
SwerveDrive::SwerveDrive(const std::shared_ptr<cpptoml::table> config,
                         const TalonMap* map, std::shared_ptr<AHRS> gyro)
    : SwerveDrive("SwerveDrive", config, map, gyro) {}

/**
 * Move all wheel azimuths to their home position.
 */
void SwerveDrive::ZeroAzimuth() {
  map_->lf_azimuth->Set(0.0);
  map_->rf_azimuth->Set(0.0);
  map_->lr_azimuth->Set(0.0);
  map_->rr_azimuth->Set(0.0);
}

/**
 * Disable driving in field-oriented mode if disired or required by hardware
 *  failure.
 */
void SwerveDrive::SetGyroDisabled(bool disabled) {
  logger_->info("setting swerve gyro to {}", disabled ? "DISABLED" : "ENABLED");
  gyro_disabled_ = disabled;
}

/**
 * Reconfigure the drive Talons for another operating mode.
 */
void SwerveDrive::SetDriveMode(const std::shared_ptr<talon::Settings> settings,
                               double setpoint_scale) {
  drive_scale_factor_ = setpoint_scale;
  settings->SetMode(map_->lf_drive);
  settings->SetMode(map_->rf_drive);
  settings->SetMode(map_->lr_drive);
  settings->SetMode(map_->rr_drive);
}

namespace {
std::string kCalibrationPath{"/home/lvuser/sidewinder_calibration.toml"};
}
/** Write out wheel zero azimuth data to ~lvuser/sidewinder_calibration.toml
 */
void SwerveDrive::WriteAzimuthCalibration() {
  std::shared_ptr<cpptoml::table> cal = cpptoml::make_table();
  auto time = std::time(nullptr);
  cal->insert("timestamp",
              cpptoml::offset_datetime::from_zoned(*std::localtime(&time)));
  cal->insert("lf", map_->lf_azimuth->GetPulseWidthPosition() & 0xFFF);
  cal->insert("rf", map_->rf_azimuth->GetPulseWidthPosition() & 0xFFF);
  cal->insert("lr", map_->lr_azimuth->GetPulseWidthPosition() & 0xFFF);
  cal->insert("rr", map_->rr_azimuth->GetPulseWidthPosition() & 0xFFF);

  std::ofstream file{kCalibrationPath, std::ofstream::trunc};
  file << (*cal);
  file.close();

  logger_->info("azimuth calibration data saved:");
  logger_->info("  lf = {}", *(cal->get_as<int>("lf")));
  logger_->info("  rf = {}", *(cal->get_as<int>("rf")));
  logger_->info("  lr = {}", *(cal->get_as<int>("lr")));
  logger_->info("  rr = {}", *(cal->get_as<int>("rr")));
}

/** Read in wheel zero azimuth data from ~lvuser/sidewinder_calibration.toml
 * and set the four azimuth quad encoders to match the current absolute offset
 * from zero.
 */
void SwerveDrive::ReadAzimuthCalibration() {
  auto cal = cpptoml::parse_file(kCalibrationPath);
  auto timestamp = *(cal->get_as<cpptoml::offset_datetime>("timestamp"));
  logger_->info("loading azimuth calibration from {}", timestamp);

  // get and verify presence of cal data
  auto value = cal->get_as<int>("lf");
  if (!value) logger_->critical("lf calibration data missing, using 0");
  int lf = value.value_or(0);

  value = cal->get_as<int>("rf");
  if (!value) logger_->critical("rf calibration data missing, using 0");
  int rf = value.value_or(0);

  value = cal->get_as<int>("lr");
  if (!value) logger_->critical("lr calibration data missing, using 0");
  int lr = value.value_or(0);

  value = cal->get_as<int>("rr");
  if (!value) logger_->critical("rr calibration data missing, using 0");
  int rr = value.value_or(0);

  // Take lower 12-bits of current absolute encoder position (0-4096) and offset
  // by our recorded zero position. Repeat for all four wheels.
  auto pos = (map_->lf_azimuth->GetPulseWidthPosition() & 0xFFF) - lf;
  map_->lf_azimuth->SetPosition(pos);
  logger_->debug("set left front azimuth position = {}", pos);

  pos = (map_->rf_azimuth->GetPulseWidthPosition() & 0xFFF) - rf;
  map_->rf_azimuth->SetPosition(pos);
  logger_->debug("set right front azimuth position = {}", pos);

  pos = (map_->lr_azimuth->GetPulseWidthPosition() & 0xFFF) - lr;
  map_->lr_azimuth->SetPosition(pos);
  logger_->debug("set left rear azimuth position = {}", pos);

  pos = (map_->rr_azimuth->GetPulseWidthPosition() & 0xFFF) - rr;
  map_->rr_azimuth->SetPosition(pos);
  logger_->debug("set right rear azimuth position = {}", pos);
}

/** Drive in field-oriented swerve drive mode.
 * @param forward command forward/backwards (Y-axis) throttle, -1.0 - 1.0
 * @param strafe command left/right (X-axis) throttle, -1.0 - 1.0
 * @param azimuth command CW/CCW azimuth throttle, -1.0 - 1.0
 */
void SwerveDrive::Drive(double forward, double strafe, double azimuth) {
  if (gyro_disabled_) {
    Drive_(forward, strafe, azimuth);
    return;
  }

  double rad = ahrs_->GetYaw() * (2.0 * M_PI) / 360.0;
  double cos_rad = cos(rad);
  double sin_rad = sin(rad);
  double rotated_forward = forward * cos_rad + strafe * sin_rad;
  double rotated_strafe = strafe * cos_rad - forward * sin_rad;
  Drive_(rotated_forward, rotated_strafe, azimuth);
}

/** Drive in swerve drive mode without gyro.
 * @param forward command forward/backwards (Y-axis) throttle, -1.0 - 1.0
 * @param strafe command left/right (X-axis) throttle, -1.0 - 1.0
 * @param azimuth command CW/CCW azimuth throttle, -1.0 - 1.0
 */
void SwerveDrive::Drive_(double forward, double strafe, double azimuth) {
  // don't reset wheels to zero in dead zone
  // FIXME: dead zone hard-coded
  bool is_stopping = std::fabs(forward) <= dead_zone &&
                     std::fabs(strafe) <= dead_zone &&
                     std::fabs(azimuth) < dead_zone;

  if (is_stopping) {
    map_->rf_drive->Set(0);
    map_->lf_drive->Set(0);
    map_->lr_drive->Set(0);
    map_->rr_drive->Set(0);
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

#if !NDEBUG
  static int i;
  if (++i == 50) {
    i = 0;
    logger_->trace("warf = {}, walf = {}, walr = {}, warr = {}", dd.warf,
                   dd.walf, dd.walr, dd.warr);
    logger_->trace("wsrf = {}, wslf = {}, wslr = {}, wsrr = {}", dd.wsrf,
                   dd.wslf, dd.wslr, dd.wsrr);
  }
#endif
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
 * @param velocity command forward/backwards (Y-axis) speed in encoder ticks per
 * 100ms
 * @param azimuth command wheel azimuth to absolute position
*/
void SwerveDrive::CrabDriveAutonomous(double velocity, int azimuth) {
  map_->lf_azimuth->Set(static_cast<double>(azimuth));
  map_->rf_azimuth->Set(static_cast<double>(azimuth));
  map_->lr_azimuth->Set(static_cast<double>(azimuth));
  map_->rr_azimuth->Set(static_cast<double>(azimuth));

  map_->lf_drive->Set(velocity);
  map_->rf_drive->Set(velocity);
  map_->lr_drive->Set(velocity);
  map_->rr_drive->Set(velocity);
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

/** Get encoder value of specified azimuth wheel.
 *  @param wheel select wheel to read
 */
int SwerveDrive::GetAzimuth(const Wheel wheel) const {
  switch (wheel) {
    case kRightRear:
      return static_cast<int>(map_->rr_azimuth->GetPosition());
    case kLeftRear:
      return static_cast<int>(map_->lr_azimuth->GetPosition());
    case kRightFront:
      return static_cast<int>(map_->rf_azimuth->GetPosition());
    case kLeftFront:
      return static_cast<int>(map_->lf_azimuth->GetPosition());
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
