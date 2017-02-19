#include "azimuth.h"

#include "robot.h"

using namespace steamworks::command;

// tuning parameters
namespace {
const double kP = 0.01;
const double kI = 0.0;
const double kD = 0.0;

const double kToleranceDegrees = 2.0;  // how close to "on target" we want to be
}

/**
 * DriveAzimuth is a command to spin the robot to a given field-relative angle.
 * See Kauai Labs example at
 * http://www.pdocs.kauailabs.com/navx-mxp/examples/rotate-to-angle-2/.
 */
DriveAzimuth::DriveAzimuth(double angle)
    : frc::Command("DriveAzimuth"),
      logger_(spdlog::get("command")),
      angle_(angle) {
  // TODO: we might want to create controller during Initialize() and destroy
  // during End() in order to stop the controller notifier
  Requires(Robot::drive);
  auto gyro = Robot::drive->GetGyro().get();  // PIDSource
  controller_.reset(new PIDController(kP, kI, kD, gyro, this));
  controller_->SetInputRange(-180.0, 180.0);
  controller_->SetOutputRange(-1.0, 1.0);
  controller_->SetAbsoluteTolerance(kToleranceDegrees);
  controller_->SetToleranceBuffer(1);
  controller_->SetContinuous(true);
}

/**
 * Initialize starts the PID controller loop.
 */
void DriveAzimuth::Initialize() {
  controller_->SetSetpoint(angle_);
  controller_->Enable();
  logger_->debug("setpoint = {}", controller_->GetSetpoint());
  logger_->debug("gyro = {}", Robot::drive->GetGyro()->GetAngle());
  logger_->debug("error = {}", controller_->GetError());
}

/**
 * Execute is called periodically during command execution and sends azimuth
 * rate commands to the swerve drive based on current PID calculations.
 */
void DriveAzimuth::Execute() {
  // if (std::fabs(azimuth_rate_) < 0.1) {
  //   if (azimuth_rate_ < 0.0) {
  //     azimuth_rate_ = -0.1;
  //   }
  //   azimuth_rate_ = 0.1;
  // }
  logger_->debug("execute azimuth rate = {:+f}", azimuth_rate_);
  Robot::drive->CartesianDrive(0.0, 0.0, azimuth_rate_);
}

/**
 * IsFinished is called periodically during command execution and returns true
 * if desired angle is reached.
 */
bool DriveAzimuth::IsFinished() {
  logger_->debug("avg. error = {}", controller_->GetAvgError());
  logger_->debug("is finished = {}", controller_->OnTarget());
  return controller_->OnTarget();
}

/**
 * End is called after IsFinished(), it stops azimuth motion and disables the
 * PID controller loop.
 */
void DriveAzimuth::End() {
  logger_->debug("end");
  Robot::drive->CartesianDrive(0.0, 0.0, 0.0);
  controller_->Reset();
  // controller_.reset(nullptr);
}

/**
 * PIDWrite is called periodically by the PID controller.
 */
void DriveAzimuth::PIDWrite(double output) { azimuth_rate_ = output; }