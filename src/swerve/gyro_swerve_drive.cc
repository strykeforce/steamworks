
#include "gyro_swerve_drive.h"
#include "AHRS.h"
#include "swerve_drive.h"

using namespace sidewinder::swerve;

constexpr double PI = 3.14159265358979323846;
constexpr double TAU = 2.0 * PI;

/** Initialize the Sidewinder SwerveDrive.
 * @param config cpptoml SIDEWINDER table
 * @param tm talon map initialized with pointers to azimuth and drive Talons
 */
GyroSwerveDrive::GyroSwerveDrive(const std::shared_ptr<cpptoml::table> config,
                                 const TalonMap* tm)
    : logger_(spdlog::stdout_logger_st("sidewinder")),
      sd_(config, tm),
      ahrs_(std::make_shared<AHRS>(SPI::Port::kMXP)),
      initial_angle_(ahrs_->GetYaw()),
      gyro_disabled_(false) {}

/** Initialize the Sidewinder SwerveDrive, reusing gyro.
 * @param config cpptoml SIDEWINDER table
 * @param tm talon map initialized with pointers to azimuth and drive Talons
 * @param gyro instance of NaxX AHRS to reuse
 */
GyroSwerveDrive::GyroSwerveDrive(const std::shared_ptr<cpptoml::table> config,
                                 const TalonMap* tm, std::shared_ptr<AHRS> gyro)
    : logger_(spdlog::stdout_logger_st("sidewinder")),
      sd_(config, tm),
      ahrs_(gyro),
      initial_angle_(ahrs_->GetYaw()),
      gyro_disabled_(false) {}

/** Drive in field-oriented swerve drive mode.
 * @param forward command forward/backwards (Y-axis) throttle, -1.0 - 1.0
 * @param strafe command left/right (X-axis) throttle, -1.0 - 1.0
 * @param azimuth command CW/CCW azimuth throttle, -1.0 - 1.0
 */
void GyroSwerveDrive::Drive(double forward, double strafe, double azimuth) {
  if (gyro_disabled_) {
    sd_.Drive(forward, strafe, azimuth);
    return;
  }

  double deg = ahrs_->GetYaw() - initial_angle_;
  double rad = deg * TAU / 360.0;
  double cos_rad = cos(rad);
  double sin_rad = sin(rad);
  double rotated_forward = forward * cos_rad + strafe * sin_rad;
  double rotated_strafe = strafe * cos_rad - forward * sin_rad;
  sd_.Drive(rotated_forward, rotated_strafe, azimuth);
}

/** Rotate around an off-center point, used for pivoting on robot shooter.
 * @param azimuth command CW/CCW azimuth throttle, -1.0 - 1.0
 */
void GyroSwerveDrive::TargetRotation(double azimuth) {
  sd_.TargetRotation(azimuth);
}

/** Get encoder value of specified drive wheel.
 *  @param wheel select wheel to read
 */
int GyroSwerveDrive::GetPosition(const SwerveDrive::Wheel wheel) const {
  return sd_.GetPosition(wheel);
}

/** Move all wheels to their home position.
 */
void GyroSwerveDrive::ZeroAzimuth() { sd_.ZeroAzimuth(); }

/** Disable driving in field-oriented mode if disired or required by hardware
 *  failure.
 */
void GyroSwerveDrive::SetGyroDisabled(bool disabled) {
  logger_->info("setting swerve gyro to {}", disabled ? "DISABLED" : "ENABLED");
  gyro_disabled_ = disabled;
}

/** Override the default logger.
 * This logger is expected to have the name "sidewinder".
 */
void GyroSwerveDrive::SetLogger(const std::shared_ptr<spdlog::logger> logger) {
  logger_ = logger;
  sd_.SetLogger(logger);
}
