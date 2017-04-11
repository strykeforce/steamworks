#include "capture_rope.h"

#include "robot.h"
#include "subsystems/climber.h"

using namespace steamworks::command::climber;

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
  Robot::climber->SetRatchetModeEnabled(true);
  climb_.Start();
}
