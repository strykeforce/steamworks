#include "health_check.h"

#include <sidewinder/swerve/talon_map.h>

#include "commands/shooter/wheel.h"
#include "robot.h"
#include "robot_map.h"
#include "subsystems/drive.h"

using namespace steamworks::command::drive;
using namespace std;

namespace {
const int kCountReq = 50;
const double kWarmUpTime = 2.0;
}  // namespace

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
  azimuth_position_iter_ = azimuth_position_.begin();
  shooter_speed_iter_ = shooter_speed_.begin();
  hopper_voltage_iter_ = hopper_voltage_.begin();
  intake_voltage_iter_ = intake_voltage_.begin();
  gear_intake_voltage_iter_ = gear_intake_voltage_.begin();
  climber_master_voltage_iter_ = climber_master_voltage_.begin();
  climber_slave_voltage_iter_ = climber_slave_voltage_.begin();
  elevation_position_iter_ = elevation_position_.begin();

  logfile_ = make_unique<ofstream>("/home/lvuser/motors.csv", ofstream::app);
  // *logfile_ << "timestamp,talon,setpoint,current,speed\n";
  is_finished_ = false;

  SetTimestamp();
  PrepareTest();
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
    return;
  }

  if (azimuth_position_iter_ != azimuth_position_.end()) {
    CheckAzimuthEncoders();
    return;
  }

  if (shooter_speed_iter_ != shooter_speed_.end()) {
    CheckShooterWheelMotor();
    return;
  }

  if (elevation_position_iter_ != elevation_position_.end()) {
    CheckElevationEncoder();
    return;
  }

  if (hopper_voltage_iter_ != hopper_voltage_.end()) {
    CheckHopperMotor();
    return;
  }

  if (intake_voltage_iter_ != intake_voltage_.end()) {
    CheckIntakeMotor();
    return;
  }

  if (gear_intake_voltage_iter_ != gear_intake_voltage_.end()) {
    CheckGearIntakeMotor();
    return;
  }

  if (climber_master_voltage_iter_ != climber_master_voltage_.end()) {
    CheckClimberMasterMotor();
    return;
  }

  if (climber_slave_voltage_iter_ != climber_slave_voltage_.end()) {
    CheckClimberSlaveMotor();
    return;
  }

  is_finished_ = true;
}

/**
 * Prepare test initilizes variables for next test.
 */
void HealthCheck::PrepareTest() {
  for (size_t i = 0; i < 4; i++) {
    drive_current_sum_[0] = 0;
    drive_speed_sum_[0] = 0;
  }
  current_sum_ = 0;
  speed_sum_ = 0;
  count_ = 0;
  is_transitioning_ = true;
  start_ = timer_.GetFPGATimestamp();
}

/**
 * Log results
 */
void HealthCheck::LogResults(int talon, double setpoint, double current,
                             double speed, double pos) {
  cout << "Talon " << talon << " at " << std::setprecision(1) << fixed
       << setpoint << " : avg current = " << std::setprecision(2) << fixed
       << current << " amps, avg speed = " << std::setprecision(0) << fixed
       << setw(4) << speed << " ticks, pos = " << pos << "\n";
  *logfile_ << log_timestamp_ << "," << talon << "," << setpoint << ","
            << current << "," << speed << "," << pos << endl;
}

/**
 * Check drive
 */
void HealthCheck::CheckDriveMotors() {
  if (timer_.GetFPGATimestamp() - start_ > kWarmUpTime) {
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
      LogResults(i + 1, *drive_voltage_iter_, drive_current_sum_[i] / kCountReq,
                 drive_speed_sum_[i] / kCountReq, 0);
    }
    drive_voltage_iter_++;
    if (drive_voltage_iter_ == drive_voltage_.end()) {
      cout << endl;
    }
    PrepareTest();
  }
  // warm uo period not done
  RobotMap::swerve_talons->lf_drive->Set(*drive_voltage_iter_);
  RobotMap::swerve_talons->rf_drive->Set(*drive_voltage_iter_);
  RobotMap::swerve_talons->rr_drive->Set(*drive_voltage_iter_);
  RobotMap::swerve_talons->lr_drive->Set(*drive_voltage_iter_);
}

/**
 * Check azimuth motors speed constants
 */
void HealthCheck::CheckAzimuthMotors() {
  if (timer_.GetFPGATimestamp() - start_ > kWarmUpTime) {
    if (!timer_.HasPeriodPassed(100 / 1000.0)) {
      return;
    }
    if (count_ < kCountReq) {
      drive_current_sum_[3] +=
          RobotMap::swerve_talons->lr_azimuth->GetOutputCurrent();
      drive_current_sum_[2] +=
          RobotMap::swerve_talons->rr_azimuth->GetOutputCurrent();
      drive_current_sum_[1] +=
          RobotMap::swerve_talons->rf_azimuth->GetOutputCurrent();
      drive_current_sum_[0] +=
          RobotMap::swerve_talons->lf_azimuth->GetOutputCurrent();

      drive_speed_sum_[0] += RobotMap::swerve_talons->lf_azimuth->GetSpeed();
      drive_speed_sum_[1] += RobotMap::swerve_talons->rf_azimuth->GetSpeed();
      drive_speed_sum_[2] += RobotMap::swerve_talons->rr_azimuth->GetSpeed();
      drive_speed_sum_[3] += RobotMap::swerve_talons->lr_azimuth->GetSpeed();

      count_++;
      return;
    }
    // done with kCountReq measurements
    for (size_t i = 0; i < 4; i++) {
      LogResults(i + 11, *azimuth_voltage_iter_,
                 drive_current_sum_[i] / kCountReq,
                 drive_speed_sum_[i] / kCountReq, 0);
    }
    azimuth_voltage_iter_++;
    if (azimuth_voltage_iter_ == azimuth_voltage_.end()) {
      cout << endl;
    }

    PrepareTest();
  }
  // warm uo period not done
  RobotMap::swerve_talons->lf_azimuth->Set(*azimuth_voltage_iter_);
  RobotMap::swerve_talons->rf_azimuth->Set(*azimuth_voltage_iter_);
  RobotMap::swerve_talons->rr_azimuth->Set(*azimuth_voltage_iter_);
  RobotMap::swerve_talons->lr_azimuth->Set(*azimuth_voltage_iter_);
}

/**
 * Check azimuth encoders
 */
void HealthCheck::CheckAzimuthEncoders() {
  if (is_transitioning_) {
    Robot::drive->SetAzimuthTestModeEnabled(false);
    // prevent multiple rotations when setting wheel position back to 0
    double pos =
        static_cast<int>(RobotMap::swerve_talons->lf_azimuth->Get()) & 0xFFF;
    RobotMap::swerve_talons->lf_azimuth->SetPosition(pos);
    pos = static_cast<int>(RobotMap::swerve_talons->rf_azimuth->Get()) & 0xFFF;
    RobotMap::swerve_talons->rf_azimuth->SetPosition(pos);
    pos = static_cast<int>(RobotMap::swerve_talons->rr_azimuth->Get()) & 0xFFF;
    RobotMap::swerve_talons->rr_azimuth->SetPosition(pos);
    pos = static_cast<int>(RobotMap::swerve_talons->lr_azimuth->Get()) & 0xFFF;
    RobotMap::swerve_talons->lr_azimuth->SetPosition(pos);

    is_transitioning_ = false;
  }
  if (timer_.GetFPGATimestamp() - start_ < kWarmUpTime + 2) {
    // warm up period not done
    RobotMap::swerve_talons->lf_azimuth->Set(*azimuth_position_iter_);
    RobotMap::swerve_talons->rf_azimuth->Set(*azimuth_position_iter_);
    RobotMap::swerve_talons->rr_azimuth->Set(*azimuth_position_iter_);
    RobotMap::swerve_talons->lr_azimuth->Set(*azimuth_position_iter_);
    return;
  }

  LogResults(11, 0, 0, 0, RobotMap::swerve_talons->lf_azimuth->Get());
  LogResults(12, 0, 0, 0, RobotMap::swerve_talons->rf_azimuth->Get());
  LogResults(13, 0, 0, 0, RobotMap::swerve_talons->rr_azimuth->Get());
  LogResults(14, 0, 0, 0, RobotMap::swerve_talons->lr_azimuth->Get());

  azimuth_position_iter_++;
  if (azimuth_position_iter_ == azimuth_position_.end()) {
    cout << endl;
  }
  PrepareTest();
}

/**
 * Check shooter wheel
 */
void HealthCheck::CheckShooterWheelMotor() {
  if (timer_.GetFPGATimestamp() - start_ < kWarmUpTime) {
    // warm up period not done
    RobotMap::shooter_wheel_talon->Set(*shooter_speed_iter_);
    return;
  }
  if (!timer_.HasPeriodPassed(100 / 1000.0)) {
    return;
  }
  if (count_ < kCountReq) {
    current_sum_ += RobotMap::shooter_wheel_talon->GetOutputCurrent();
    speed_sum_ += RobotMap::shooter_wheel_talon->GetSpeed();
    count_++;
    return;
  }
  // done with kCountReq measurements
  LogResults(21, *shooter_speed_iter_, current_sum_ / kCountReq,
             speed_sum_ / kCountReq, 0);
  current_sum_ = 0;
  speed_sum_ = 0;

  shooter_speed_iter_++;
  if (shooter_speed_iter_ == shooter_speed_.end()) {
    frc::Command *stop_wheel = new steamworks::command::StopShooterWheel();
    stop_wheel->Start();
    cout << endl;
  }
  PrepareTest();
}

/**
 * Check hopper
 */
void HealthCheck::CheckHopperMotor() {
  if (timer_.GetFPGATimestamp() - start_ < kWarmUpTime) {
    // warm up period not done
    RobotMap::hopper_talon->Set(*hopper_voltage_iter_);
    return;
  }
  if (!timer_.HasPeriodPassed(100 / 1000.0)) {
    return;
  }
  if (count_ < kCountReq) {
    current_sum_ += RobotMap::hopper_talon->GetOutputCurrent();
    speed_sum_ += RobotMap::hopper_talon->GetSpeed();
    count_++;
    return;
  }
  // done with kCountReq measurements
  LogResults(RobotMap::kHopper, *hopper_voltage_iter_, current_sum_ / kCountReq,
             -speed_sum_ / kCountReq, 0);
  current_sum_ = 0;
  speed_sum_ = 0;

  hopper_voltage_iter_++;
  if (hopper_voltage_iter_ == hopper_voltage_.end()) {
    RobotMap::hopper_talon->Set(0);
    cout << endl;
  }
  PrepareTest();
}

/**
 * Check intake
 */
void HealthCheck::CheckIntakeMotor() {
  if (timer_.GetFPGATimestamp() - start_ < kWarmUpTime) {
    // warm up period not done
    RobotMap::intake_talon->Set(*intake_voltage_iter_);
    return;
  }
  if (!timer_.HasPeriodPassed(100 / 1000.0)) {
    return;
  }
  if (count_ < kCountReq) {
    current_sum_ += RobotMap::intake_talon->GetOutputCurrent();
    speed_sum_ += RobotMap::intake_talon->GetSpeed();
    count_++;
    return;
  }
  // done with kCountReq measurements
  LogResults(RobotMap::kIntake, *intake_voltage_iter_, current_sum_ / kCountReq,
             speed_sum_ / kCountReq, 0);
  current_sum_ = 0;
  speed_sum_ = 0;

  intake_voltage_iter_++;
  if (intake_voltage_iter_ == intake_voltage_.end()) {
    RobotMap::intake_talon->Set(0);
    cout << endl;
  }
  PrepareTest();
}

/**
 * Check gear intake
 */
void HealthCheck::CheckGearIntakeMotor() {
  if (timer_.GetFPGATimestamp() - start_ < kWarmUpTime - 1) {
    // warm up period not done
    RobotMap::gear_intake_talon->Set(*gear_intake_voltage_iter_);
    return;
  }
  if (!timer_.HasPeriodPassed(100 / 1000.0)) {
    return;
  }
  if (count_ < kCountReq) {
    current_sum_ += RobotMap::gear_intake_talon->GetOutputCurrent();
    speed_sum_ += RobotMap::gear_intake_talon->GetSpeed();
    count_++;
    return;
  }
  // done with kCountReq measurements
  LogResults(RobotMap::kGearIntake, *gear_intake_voltage_iter_,
             current_sum_ / kCountReq, speed_sum_ / kCountReq, 0);
  current_sum_ = 0;
  speed_sum_ = 0;

  gear_intake_voltage_iter_++;
  if (gear_intake_voltage_iter_ == gear_intake_voltage_.end()) {
    RobotMap::gear_intake_talon->Set(0);
    cout << endl;
  }
  PrepareTest();
}

/**
 * Check climber master
 */
void HealthCheck::CheckClimberMasterMotor() {
  if (timer_.GetFPGATimestamp() - start_ < kWarmUpTime - 1) {
    // warm up period not done
    RobotMap::climber_slave_talon->ConfigNeutralMode(
        CANSpeedController::kNeutralMode_Coast);
    RobotMap::climber_master_talon->Set(*climber_master_voltage_iter_);
    return;
  }
  if (!timer_.HasPeriodPassed(100 / 1000.0)) {
    return;
  }
  if (count_ < kCountReq) {
    current_sum_ += RobotMap::climber_master_talon->GetOutputCurrent();
    speed_sum_ += RobotMap::climber_master_talon->GetSpeed();
    count_++;
    return;
  }
  // done with kCountReq measurements
  LogResults(RobotMap::kClimberMaster, *climber_master_voltage_iter_,
             current_sum_ / kCountReq, -speed_sum_ / kCountReq, 0);
  current_sum_ = 0;
  speed_sum_ = 0;

  climber_master_voltage_iter_++;
  if (climber_master_voltage_iter_ == climber_master_voltage_.end()) {
    RobotMap::climber_master_talon->Set(0);
    RobotMap::climber_slave_talon->ConfigNeutralMode(
        CANSpeedController::kNeutralMode_Brake);
    cout << endl;
  }
  PrepareTest();
}

/**
 * Check climber slave
 */
void HealthCheck::CheckClimberSlaveMotor() {
  if (timer_.GetFPGATimestamp() - start_ < kWarmUpTime) {
    // warm up period not done
    RobotMap::climber_master_talon->ConfigNeutralMode(
        CANSpeedController::kNeutralMode_Coast);
    RobotMap::climber_slave_talon->Set(*climber_slave_voltage_iter_);
    return;
  }
  if (!timer_.HasPeriodPassed(100 / 1000.0)) {
    return;
  }
  if (count_ < kCountReq) {
    current_sum_ += RobotMap::climber_slave_talon->GetOutputCurrent();
    speed_sum_ += RobotMap::climber_slave_talon->GetSpeed();
    count_++;
    return;
  }
  // done with kCountReq measurements
  LogResults(RobotMap::kClimberSlave, *climber_slave_voltage_iter_,
             current_sum_ / kCountReq, speed_sum_ / kCountReq, 0);
  current_sum_ = 0;
  speed_sum_ = 0;

  climber_slave_voltage_iter_++;
  if (climber_slave_voltage_iter_ == climber_slave_voltage_.end()) {
    RobotMap::climber_slave_talon->Set(0);
    RobotMap::climber_master_talon->ConfigNeutralMode(
        CANSpeedController::kNeutralMode_Brake);
    cout << endl;
  }
  PrepareTest();
}

/**
 * Check elevation encoder
 */
void HealthCheck::CheckElevationEncoder() {
  if (is_transitioning_) {
    is_transitioning_ = false;
  }
  if (timer_.GetFPGATimestamp() - start_ < kWarmUpTime - 1) {
    // warm up period not done
    RobotMap::shooter_elevation_talon->Set(*elevation_position_iter_);
    return;
  }

  LogResults(RobotMap::kShooterElevation, *elevation_position_iter_, 0, 0,
             RobotMap::shooter_elevation_talon->Get());

  elevation_position_iter_++;
  if (elevation_position_iter_ == elevation_position_.end()) {
    cout << endl;
  }
  PrepareTest();
}

/**
 * IsFinished
 */
bool HealthCheck::IsFinished() { return is_finished_; }

/**
 * finis
 */
void HealthCheck::End() {
  logfile_->close();
  Robot::drive->SetAzimuthTestModeEnabled(false);
}

/**
 * Construct the timestamp for the test run.
 */
void HealthCheck::SetTimestamp() {
  char ts[20];
  time_t t = std::time(nullptr);
  strftime(ts, sizeof(ts), "%F %T", localtime(&t));
  log_timestamp_ = std::string(ts);
}
