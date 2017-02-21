#include "clamp.h"

#include "robot.h"

using namespace steamworks::command::gear;

namespace {
const double kServoPosition = 0.3;
const double kServoCloseTime = 1.0;
}

CloseClamp::CloseClamp() : frc::TimedCommand("CloseClamp", kServoCloseTime) {
  Requires(Robot::gear_loader);
}

void CloseClamp::Initialize() { Robot::gear_loader->SetServo(0.5); }
