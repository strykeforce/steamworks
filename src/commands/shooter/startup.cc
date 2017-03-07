#include "startup.h"

#include "robot.h"

using namespace steamworks::command::shooter;

namespace {
const double kTimeout = 2.5;
const int kSpeedGoodEnough = 25;
const int kElevationGoodEnough = 10;
}

StartupShot::StartupShot(int speed, int elevation)
    : frc::Command("StartupShot", kTimeout),
      logger_(spdlog::get("command")),
      speed_(speed),
      elevation_(elevation) {
  Requires(Robot::shooter);
}

void StartupShot::Initialize() {
  Robot::shooter->SetSpeed(speed_);
  Robot::shooter->SetElevation(elevation_);
}

bool StartupShot::IsFinished() {
  int speed_error = std::abs(speed_ - Robot::shooter->GetSpeed());
  int elevation_error = std::abs(elevation_ - Robot::shooter->GetElevation());
  return ((speed_error < kSpeedGoodEnough) &&
          (elevation_error < kElevationGoodEnough)) ||
         IsTimedOut();
}

void StartupShot::End() {
  SPDLOG_DEBUG(logger_, "StartupShot finished, time since initialization: {}",
               TimeSinceInitialized());
}
