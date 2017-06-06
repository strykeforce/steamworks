#include "health_check.h"

#include <sidewinder/swerve/talon_map.h>

#include "robot.h"
#include "robot_map.h"
#include "subsystems/drive.h"

using namespace steamworks::command::drive;
using namespace std;

namespace {
const int kCountReq = 50;
}

HealthCheck::HealthCheck() : frc::Command("HealthCheck") {
  Requires(Robot::drive);
}

/**
* Initialize
*/
void HealthCheck::Initialize() {
  Robot::drive->SetTeleOpMode();
  Robot::drive->SetAzimuthTestModeEnabled(true);
  cout << "*** Checking Drive and Azimuth motors ***" << endl;
  drive_voltage_iter_ = drive_voltage_.begin();
  azimuth_voltage_iter_ = azimuth_voltage_.begin();
  count_ = 0;
  start_ = timer_.GetFPGATimestamp();
  timer_.Start();
}

/**
* Execute
*/
void HealthCheck::Execute() {
  if (drive_voltage_iter_ != drive_voltage_.end()) {
    CheckDriveMotors();
    return;
  }

  if (azimuth_voltage_iter_ != azimuth_voltage_.end()) {
    CheckAzimuthMotors();
  }
}

/**
* Check drive
*/
void HealthCheck::CheckDriveMotors() {
  if (timer_.GetFPGATimestamp() - start_ > 3.0) {
    if (!timer_.HasPeriodPassed(100 / 1000.0)) {
      return;
    }
    if (count_ < kCountReq) {
      drive_current_sum_[3] +=
          RobotMap::swerve_talons->lr_drive->GetOutputCurrent();
      drive_current_sum_[2] +=
          RobotMap::swerve_talons->rr_drive->GetOutputCurrent();
      drive_current_sum_[1] +=
          RobotMap::swerve_talons->rf_drive->GetOutputCurrent();
      drive_current_sum_[0] +=
          RobotMap::swerve_talons->lf_drive->GetOutputCurrent();

      drive_speed_sum_[0] += RobotMap::swerve_talons->lf_drive->GetSpeed();
      drive_speed_sum_[1] += RobotMap::swerve_talons->rf_drive->GetSpeed();
      drive_speed_sum_[2] += RobotMap::swerve_talons->rr_drive->GetSpeed();
      drive_speed_sum_[3] += RobotMap::swerve_talons->lr_drive->GetSpeed();

      count_++;
      return;
    }
    // done with kCountReq measurements
    for (size_t i = 0; i < 4; i++) {
      cout << "Drive " << i + 1 << " at " << std::setprecision(0) << fixed
           << *drive_voltage_iter_
           << " volts: avg current = " << std::setprecision(2) << fixed
           << drive_current_sum_[i] / kCountReq
           << " amps, avg speed = " << std::setprecision(0) << fixed << setw(4)
           << drive_speed_sum_[i] / kCountReq << " ticks/100ms\n";
      drive_current_sum_[i] = 0;
      drive_speed_sum_[i] = 0;
    }
    cout << endl;
    drive_voltage_iter_++;
    count_ = 0;
    start_ = timer_.GetFPGATimestamp();
  }
  // warm uo period not done
  RobotMap::swerve_talons->lf_drive->Set(*drive_voltage_iter_);
  RobotMap::swerve_talons->rf_drive->Set(*drive_voltage_iter_);
  RobotMap::swerve_talons->rr_drive->Set(*drive_voltage_iter_);
  RobotMap::swerve_talons->lr_drive->Set(*drive_voltage_iter_);
}

/**
* Check azimuth
*/
void HealthCheck::CheckAzimuthMotors() {
  if (timer_.GetFPGATimestamp() - start_ > 3.0) {
    if (!timer_.HasPeriodPassed(100 / 1000.0)) {
      return;
    }
    if (count_ < kCountReq) {
      azimuth_current_sum_[3] +=
          RobotMap::swerve_talons->lr_azimuth->GetOutputCurrent();
      azimuth_current_sum_[2] +=
          RobotMap::swerve_talons->rr_azimuth->GetOutputCurrent();
      azimuth_current_sum_[1] +=
          RobotMap::swerve_talons->rf_azimuth->GetOutputCurrent();
      azimuth_current_sum_[0] +=
          RobotMap::swerve_talons->lf_azimuth->GetOutputCurrent();

      azimuth_speed_sum_[0] += RobotMap::swerve_talons->lf_azimuth->GetSpeed();
      azimuth_speed_sum_[1] += RobotMap::swerve_talons->rf_azimuth->GetSpeed();
      azimuth_speed_sum_[2] += RobotMap::swerve_talons->rr_azimuth->GetSpeed();
      azimuth_speed_sum_[3] += RobotMap::swerve_talons->lr_azimuth->GetSpeed();

      count_++;
      return;
    }
    // done with kCountReq measurements
    for (size_t i = 0; i < 4; i++) {
      cout << "Azimuth " << i + 1 << " at " << std::setprecision(0) << fixed
           << *azimuth_voltage_iter_
           << " volts: avg current = " << std::setprecision(2) << fixed
           << azimuth_current_sum_[i] / kCountReq
           << " amps, avg speed = " << std::setprecision(0) << fixed << setw(4)
           << azimuth_speed_sum_[i] / kCountReq << " ticks/100ms\n";
      azimuth_current_sum_[i] = 0;
      azimuth_speed_sum_[i] = 0;
    }
    cout << endl;
    azimuth_voltage_iter_++;
    count_ = 0;
    start_ = timer_.GetFPGATimestamp();
  }
  // warm uo period not done
  RobotMap::swerve_talons->lf_azimuth->Set(*azimuth_voltage_iter_);
  RobotMap::swerve_talons->rf_azimuth->Set(*azimuth_voltage_iter_);
  RobotMap::swerve_talons->rr_azimuth->Set(*azimuth_voltage_iter_);
  RobotMap::swerve_talons->lr_azimuth->Set(*azimuth_voltage_iter_);
}
/**
* IsFinished
*/
bool HealthCheck::IsFinished() {
  return azimuth_voltage_iter_ == azimuth_voltage_.end();
}

/**
 * finis
 */
void HealthCheck::End() { Robot::drive->SetAzimuthTestModeEnabled(false); }
