
#include "gyro_swerve_drive.h"
#include "AHRS.h"
#include "swerve_drive.h"

using namespace sidewinder::swerve;

constexpr double PI = 3.14159265358979323846;
constexpr double TAU = 2.0 * PI;

GyroSwerveDrive::GyroSwerveDrive(const std::shared_ptr<cpptoml::table> config,
                                 const TalonMap* tm)
    : sd(config, tm),
      ahrs(SPI::Port::kMXP)

{
  first_time = true;
}
void GyroSwerveDrive::ZeroAzimuth() { sd.ZeroAzimuth(); }
void GyroSwerveDrive::Drive(double forward, double strafe, double azimuth) {
  if (first_time) {
    inital_angle = ahrs.GetYaw();
    first_time = false;
  }
  double deg = ahrs.GetYaw() - inital_angle;
  double rad = deg * TAU / 360.0;
  double cos_rad = cos(rad);
  double sin_rad = sin(rad);
  double rotated_forward = forward * cos_rad + strafe * sin_rad;
  double rotated_strafe = strafe * cos_rad - forward * sin_rad;
  sd.Drive(rotated_forward, rotated_strafe, azimuth);
}
