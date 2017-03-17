#include "clamp.h"

#include "robot.h"
#include "subsystems/gear.h"

using namespace steamworks::command::gear;

namespace {
const double kServoCloseTime = 0.75;
}

CloseClamp::CloseClamp() : frc::TimedCommand("CloseClamp", kServoCloseTime) {
  Requires(Robot::gear_loader);
}

void CloseClamp::Initialize() { Robot::gear_loader->ClampShut(); }
