#pragma once

#include <WPILib.h>
#include <array>

namespace steamworks {
namespace command {
namespace drive {
class HealthCheck : public frc::Command {
 public:
  HealthCheck();
  virtual ~HealthCheck() = default;

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;

 private:
  std::array<double, 2> drive_voltage_ = {{2.0, 12.0}};
  std::array<double, 2>::iterator drive_voltage_iter_;

  std::array<double, 1> azimuth_voltage_ = {{6}};
  std::array<double, 1>::iterator azimuth_voltage_iter_;

  int count_;
  std::array<double, 4> drive_current_sum_;
  std::array<double, 4> drive_speed_sum_;
  std::array<double, 4> azimuth_current_sum_;
  std::array<double, 4> azimuth_speed_sum_;

  frc::Timer timer_;
  double start_;

  void CheckDriveMotors();
  void CheckAzimuthMotors();
};
} /* drive */
} /* command */
} /* steamworks */
