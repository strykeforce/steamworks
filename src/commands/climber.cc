#include "climber.h"

#include "robot.h"
#include "subsystems/climber.h"

using namespace steamworks::command;

//
// CaptureRope
///
CaptureRope::CaptureRope()
    : frc::Command("CaptureRope"), logger_(spdlog::get("command")) {
  Requires(Robot::climber);
  SetInterruptible(true);
}

void CaptureRope::Initialize() {
  logger_->debug("starting climber");
  Robot::climber->SetCaptureModeEnabled(true);
  Robot::climber->StartCapture();
}

bool CaptureRope::IsFinished() { return Robot::climber->IsCaptured(); }

void CaptureRope::Interrupted() {
  logger_->debug("capture is interrupted");
  End();
}

void CaptureRope::End() {
  if (IsCanceled()) {
    logger_->info("rope capture is interrupted");
    return;
  }
  logger_->info("rope is captured");
  Robot::climber->SetCaptureModeEnabled(false);
  climb_.Start();
}

//
// StartClimb
///
StartClimb::StartClimb()
    : frc::InstantCommand("StartClimb"), logger_(spdlog::get("command")) {
  Requires(Robot::climber);
  SetInterruptible(true);
}

void StartClimb::Initialize() {
  logger_->info("starting climber");
  Robot::climber->StartClimb();
}

//
// StopClimb
///
StopClimb::StopClimb()
    : frc::InstantCommand("StopClimb"), logger_(spdlog::get("command")) {
  Requires(Robot::climber);
  SetInterruptible(false);
}

void StopClimb::Initialize() {
  logger_->info("stopping climber");
  Robot::climber->Stop();
}