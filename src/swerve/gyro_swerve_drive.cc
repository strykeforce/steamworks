
#include "gyro_swerve_drive.h"
#include "AHRS.h"
#include "swerve_drive.h"

using namespace sidewinder::swerve;

constexpr double PI = 3.14159265358979323846;
constexpr double TAU = 2.0 * PI;

GyroSwerveDrive::GyroSwerveDrive(const std::shared_ptr<cpptoml::table> config,
                                 const TalonMap* tm)
    : sd_(config, tm),
      ahrs_(std::make_shared<AHRS>(SPI::Port::kMXP)),
      initial_angle_(0.0),
      first_time_(true) {}

GyroSwerveDrive::GyroSwerveDrive(const std::shared_ptr<cpptoml::table> config,
                                 const TalonMap* tm, std::shared_ptr<AHRS> gyro)
    : sd_(config, tm), ahrs_(gyro), initial_angle_(0.0), first_time_(true) {}

void GyroSwerveDrive::ZeroAzimuth() { sd_.ZeroAzimuth(); }

void GyroSwerveDrive::Drive(double forward, double strafe, double azimuth) {
  // TODO: can we move this to constructor?
  if (first_time_) {
    initial_angle_ = ahrs_->GetYaw();
    first_time_ = false;
  }

  double deg = ahrs_->GetYaw() - initial_angle_;
  double rad = deg * TAU / 360.0;
  double cos_rad = cos(rad);
  double sin_rad = sin(rad);
  double rotated_forward = forward * cos_rad + strafe * sin_rad;
  double rotated_strafe = strafe * cos_rad - forward * sin_rad;
  sd_.Drive(rotated_forward, rotated_strafe, azimuth);
}
