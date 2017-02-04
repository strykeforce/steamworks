#include "trim.h"

#include "spdlog/spdlog.h"

#include "oi.h"

using namespace steamworks::trigger;

Trim::Trim(Direction direction)
    : frc::Trigger(), angle_(0), gamepad_(OI::kGamePadJoystick) {
  switch (direction) {
    case kUp:
      angle_ = 0;
      break;
    case kRight:
      angle_ = 90;
      break;
    case kDown:
      angle_ = 180;
      break;
    case kLeft:
      angle_ = 270;
      break;
  }
}

bool Trim::Get() { return gamepad_.GetPOV() == angle_; }
