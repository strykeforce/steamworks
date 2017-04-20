#include "set_shooter.h"

#include "robot.h"
#include "subsystems/shooter_elevation.h"
#include "subsystems/shooter_wheel.h"

using namespace steamworks::command::shooter;

SetShooter::SetShooter(int speed, int elevation)
    : frc::InstantCommand("SetShooter"),
      logger_(spdlog::get("command")),
      speed_(speed),
      elevation_(elevation) {}

void SetShooter::Initialize() {
  Robot::shooter_wheel->SetSpeed(speed_);
  Robot::shooter_elevation->SetElevation(elevation_);
  logger_->info("SetShooter initialized with speed {} and elevation {}", speed_,
                elevation_);
}
