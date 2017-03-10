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
  SPDLOG_DEBUG(logger_, "CaptureRope starting climber");
  Robot::climber->SetCaptureModeEnabled(true);
  Robot::climber->StartCapture();
}

bool CaptureRope::IsFinished() { return Robot::climber->IsCaptured(); }

void CaptureRope::Interrupted() {
  SPDLOG_DEBUG(logger_, "CaptureRope capture is interrupted");
  Robot::climber->SetCaptureModeEnabled(false);
}

void CaptureRope::End() {
  logger_->info("CaptureRope rope is captured");
  Robot::climber->SetCaptureModeEnabled(false);
  Robot::climber->ZeroPosition();
  climb_.Start();
}

//
// StartClimb
///
StartClimb::StartClimb()
    : frc::Command("StartClimb"), logger_(spdlog::get("command")) {
  Requires(Robot::climber);
  SetInterruptible(true);
}

/**
 * Start the climb
 */
void StartClimb::Initialize() {
  logger_->info("StartClimb starting climber with position {}",
                Robot::climber->GetPosition());
  Robot::climber->StartClimb();
}

/**
 * Finished when we have traveled desired distance up rope.
 */
bool StartClimb::IsFinished() { return Robot::climber->ShouldClimbFinish(); }

/**
 * Button released
 */
void StartClimb::Interrupted() {
  SPDLOG_DEBUG(logger_, "StartClimb climb is interrupted");
}

/**
 * Done with climb
 */
void StartClimb::End() {
  logger_->info("StartClimb finished rope climb");
  finish_.Start();
}

//
// FinishClimb
//
FinishClimb::FinishClimb()
    : frc::InstantCommand("FinishClimb"), logger_(spdlog::get("command")) {
  Requires(Robot::climber);
  SetInterruptible(true);
}

/**
 * FinishClimb
 */
void FinishClimb::Initialize() {
  SPDLOG_DEBUG(logger_, "FinishClimb initialized with position {}",
               Robot::climber->GetPosition());
  Robot::climber->SetFinishModeEnabled(true);
  Robot::climber->StartFinish();
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
  logger_->info("StopClimb stopping climber with position {}",
                Robot::climber->GetPosition());
  Robot::climber->Stop();
}
