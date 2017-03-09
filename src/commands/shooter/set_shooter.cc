#include "set_shooter.h"

#include "robot.h"

using namespace steamworks::command::shooter;

namespace {
const double kTimeout = 2.5;
const int kSpeedGoodEnough = 5;
const int kElevationGoodEnough = 10;
}

SetShooter::SetShooter(int speed, int elevation)
    : frc::Command("SetShooter", kTimeout),
      logger_(spdlog::get("command")),
      speed_(speed),
      elevation_(elevation) {
  Requires(Robot::shooter_elevation);
  Requires(Robot::shooter_wheel);
}

void SetShooter::Initialize() {
  Robot::shooter_wheel->SetSpeed(speed_);
  Robot::shooter_elevation->SetElevation(elevation_);
  logger_->info("SetShooter initialized with speed {} and elevation {}", speed_,
                elevation_);
}

bool SetShooter::IsFinished() {
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
void SetShooter::Interrupted() {
  logger_->info("SetShooter interrupted");
  End();
}

/**
 * Called once when ending.
 */
void SetShooter::End() {
  logger_->info("SetShooter ended with speed {} and elevation {}",
                Robot::shooter_wheel->GetSpeed(),
                Robot::shooter_elevation->GetElevation());
}
