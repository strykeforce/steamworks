#include "auton.h"

#include "oi.h"
#include "robot.h"
#include "subsystems/drive.h"

using namespace steamworks::command;

/**
 * DriveAutonomous is a WPI command-based command to autonomously drive the
 * Sidewinder swerve drive.
 */
DriveAutonomous::DriveAutonomous(double forward, double strafe, double azimuth,
                                 int distance)
    : frc::Command("DriveAutonomous"),
      logger_(spdlog::get("command")),
      initial_pos_(0),
      distance_(distance),
      forward_(forward),
      strafe_(strafe),
      azimuth_(azimuth) {
  Requires(Robot::drive);
}

void DriveAutonomous::Initialize() {
  Robot::drive->SetAutonMode();
  initial_pos_ = Robot::drive->GetPosition();
}

void DriveAutonomous::Execute() {
  Robot::drive->DriveAutonomous(forward_, strafe_, azimuth_);
}

bool DriveAutonomous::IsFinished() {
  int current_pos = Robot::drive->GetPosition();
  logger_->debug("initial_pos_ = {} current_pos = {}", initial_pos_,
                 current_pos);
  return std::abs(current_pos - initial_pos_) > distance_;
}

void DriveAutonomous::End() { Robot::drive->Drive(0.0, 0.0, 0.0); }

namespace {
const int kForward = static_cast<int>(84 * 48.89);
const int kStrafe = static_cast<int>(72 * 48.89);
}

AutonTestSeq::AutonTestSeq() : frc::CommandGroup("AutonTestSeq") {
  AddSequential(new Log("Driving Forward"));
  AddSequential(new DriveAutonomous(0.15, 0.0, 0.0, kForward));

  AddSequential(new Log("Driving Sideways"));
  AddSequential(new DriveAutonomous(0.0, -0.15, 0.0, kStrafe));
}
