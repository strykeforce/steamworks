#include "auton.h"

#include "oi.h"
#include "robot.h"
#include "subsystems/drive.h"

using namespace steamworks::command;

namespace {
const int kAzimuthTolerance = 57;  // about 5 degrees
}

/**
 * Set wheel direction for next segment in autonomous.
 *
 * @param azimuth wheel position in encoder ticks, -2048 - 2048
 */
PositionWheels::PositionWheels(int azimuth)
    : frc::Command("PositionWheels"),
      logger_(spdlog::get("command")),
      azimuth_(azimuth) {
  Requires(Robot::drive);
}

/**
 * Sends the command to azimuth the wheels to the desired position.
 */
void PositionWheels::Initialize() {
  logger_->info("positioning wheels to {}", azimuth_);
  Robot::drive->CrabDriveAutonomous(0, azimuth_);
}

/**
 * Waits for wheel azimuths to be in position.
 */
bool PositionWheels::IsFinished() {
  logger_->debug("position wheels error {}",
                 std::abs(azimuth_ - Robot::drive->GetAzimuth()));
  return std::abs(azimuth_ - Robot::drive->GetAzimuth()) < kAzimuthTolerance;
}

//
// DriveAutonomous
//

/**
 * DriveAutonomous is a WPI command-based command to autonomously drive the
 * Sidewinder swerve drive.
 *
 * @param azimuth direction to point all four wheels
 */
DriveAutonomous::DriveAutonomous(int velocity, int azimuth, int distance)
    : frc::Command("DriveAutonomous"),
      logger_(spdlog::get("command")),
      velocity_(velocity),
      azimuth_(azimuth),
      distance_(distance) {
  Requires(Robot::drive);
}

void DriveAutonomous::Initialize() {
  logger_->info("auton drive direction {} at {} ticks/100ms for {} ticks",
                azimuth_, velocity_, distance_);

  Robot::drive->SetAutonMode();
  initial_pos_ = Robot::drive->GetPosition();
}

void DriveAutonomous::Execute() {
  Robot::drive->CrabDriveAutonomous(velocity_, azimuth_);
}

bool DriveAutonomous::IsFinished() {
  int current_pos = Robot::drive->GetPosition();
  int error = distance_ - std::abs(current_pos - initial_pos_);
  logger_->debug("initial_pos_ = {} current_pos = {} error = {}", initial_pos_,
                 current_pos, error);
  return std::abs(error) < 100;
}

void DriveAutonomous::End() { Robot::drive->CrabDriveAutonomous(0, azimuth_); }

DriveProfile::DriveProfile(int distance)
    : frc::Command("DriveProfile"),
      logger_(spdlog::get("command")),
      distance_(distance) {
  Requires(Robot::drive);
}

void DriveProfile::Initialize() {
  logger_->debug("initializing drive profile command");
  Robot::drive->SetAutonMode();
  logger_->debug("drive profile command auton mode set");
  Robot::drive->DriveDistance(distance_);
  logger_->debug("drive profile command motion profile started");
}

bool DriveProfile::IsFinished() {
  auto done = Robot::drive->IsMotionDone();
  logger_->debug("drive profile command done = {}", done);
  return done;
}

//
// AutonTestSeq
//

namespace {
const int kForwardAzimuth = 2048;
const int kStrafeAzimuth = 4096 - 1024;
const int kForwardDistance = static_cast<int>(76 * 48.89);
const int kStrafeDistance = static_cast<int>(60 * 48.89);
const int kVelocity = 200;
}

/**
 * Command group for auton teting
 */
AutonTestSeq::AutonTestSeq()
    : frc::CommandGroup("AutonTestSeq"), logger_(spdlog::get("command")) {
  AddSequential(new DriveProfile(4000));
  // AddSequential(new PositionWheels(kForwardAzimuth));
  //
  // AddSequential(
  //     new DriveAutonomous(kVelocity, kForwardAzimuth, kForwardDistance));
  //
  // AddSequential(new PositionWheels(kStrafeAzimuth));
  //
  // AddSequential(
  //     new DriveAutonomous(kVelocity, kStrafeAzimuth, kStrafeDistance));
}
