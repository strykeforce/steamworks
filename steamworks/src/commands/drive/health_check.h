#pragma once

#include <WPILib.h>
#include <array>
#include <fstream>

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
  std::array<double, 2> drive_voltage_ = {{2, 11.5}};
  std::array<double, 2>::iterator drive_voltage_iter_;

  std::array<double, 1> azimuth_voltage_ = {{6}};
  std::array<double, 1>::iterator azimuth_voltage_iter_;

  std::array<double, 1> azimuth_position_ = {{0}};
  std::array<double, 1>::iterator azimuth_position_iter_;

  int count_;
  std::array<double, 4> drive_current_sum_;
  std::array<double, 4> drive_speed_sum_;

  std::array<double, 2> shooter_speed_ = {{450, 500}};
  std::array<double, 2>::iterator shooter_speed_iter_;

  std::array<double, 3> hopper_voltage_ = {{4, 8, 11.5}};
  std::array<double, 3>::iterator hopper_voltage_iter_;

  std::array<double, 1> intake_voltage_ = {{11.5}};
  std::array<double, 1>::iterator intake_voltage_iter_;

  std::array<double, 2> gear_intake_voltage_ = {{6, 11.5}};
  std::array<double, 2>::iterator gear_intake_voltage_iter_;

  std::array<double, 2> climber_master_voltage_ = {{4, 11.5}};
  std::array<double, 2>::iterator climber_master_voltage_iter_;

  std::array<double, 2> climber_slave_voltage_ = {{4, 11.5}};
  std::array<double, 2>::iterator climber_slave_voltage_iter_;

  std::array<double, 9> elevation_position_ = {
      {0, 100, 2000, 3000, 4000, 3000, 2000, 1000, 0}};
  std::array<double, 9>::iterator elevation_position_iter_;

  double current_sum_;
  double speed_sum_;

  bool is_finished_ = false;
  bool is_transitioning_ = true;

  frc::Timer timer_;
  double start_;

  std::unique_ptr<std::ofstream> logfile_;
  std::string log_timestamp_;

  void PrepareTest();
  void CheckDriveMotors();
  void CheckAzimuthMotors();
  void CheckAzimuthEncoders();
  void CheckShooterWheelMotor();
  void CheckHopperMotor();
  void CheckIntakeMotor();
  void CheckGearIntakeMotor();
  void CheckClimberMasterMotor();
  void CheckClimberSlaveMotor();
  void CheckElevationEncoder();

  void SetTimestamp();
  void LogResults(int talon, double setpoint, double current, double speed,
                  double pos);
};
}  // namespace drive
}  // namespace command
}  // namespace steamworks
