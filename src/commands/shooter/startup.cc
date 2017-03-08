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
  Requires(Robot::shooter_elevation);
  Requires(Robot::shooter_wheel);
}

void StartupShot::Initialize() {
  Robot::shooter_wheel->SetSpeed(speed_);
  Robot::shooter_elevation->SetElevation(elevation_);
  logger_->info("StartupShot initialized with speed {} and elevation {}",
                speed_, elevation_);
}

bool StartupShot::IsFinished() {
  int speed_error = std::abs(speed_ - Robot::shooter_wheel->GetSpeed());
  int elevation_error =
      std::abs(elevation_ - Robot::shooter_elevation->GetElevation());
  return ((speed_error < kSpeedGoodEnough) &&
          (elevation_error < kElevationGoodEnough)) ||
         IsTimedOut();
}

/**
 * Called once if this command is interrupted.
 */
void StartupShot::Interrupted() {
  logger_->info("StartupShot interrupted");
  End();
}

/**
 * Called once when ending.
 */
void StartupShot::End() {
  logger_->info("StartupShot ended with speed {} and elevation {}",
                Robot::shooter_wheel->GetSpeed(),
                Robot::shooter_elevation->GetElevation());
}
