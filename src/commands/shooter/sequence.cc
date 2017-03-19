#include "sequence.h"

#include "commands/deadeye/shooter_led.h"
#include "commands/drive/auton/deadeye_azimuth.h"
#include "commands/hopper.h"
#include "commands/intake.h"
#include "get_angle.h"
#include "robot.h"
#include "set_elevation.h"
#include "set_shooter.h"
#include "set_wheel.h"
#include "wheel.h"

using namespace steamworks::command;

namespace {
const int kPrepareSpeed = 400;
const int kPrepareElevation = 1800;
const int kCloseShotSpeed = 440;
const int kCloseShotElevation = 50;
const double kCloseShotHopperVoltage = 6.0;
}

StartShooting::StartShooting()
    : frc::CommandGroup("StartShooting"), logger_(spdlog::get("command")) {
  SetInterruptible(true);
  AddParallel(new shooter::SetShooter(kPrepareSpeed, kPrepareElevation));
  AddSequential(new deadeye::ShooterLED(true));

  AddParallel(new drive::DeadeyeAzimuth());
  AddSequential(new shooter::GetAngle());

  AddParallel(new deadeye::ShooterLED(false));
  AddParallel(new shooter::SetElevation());
  AddSequential(new shooter::SetWheel());

  AddSequential(new StartHopper());
}

/**
 * Called if shooting sequence cancelled by button or bad solution.
 */
void StartShooting::Interrupted() {
  logger_->warn("StartShooting shooting sequence cancelled");
  End();
}

//
// StartCloseShot
//
StartCloseShot::StartCloseShot()
    : frc::CommandGroup("StartCloseShot"), logger_(spdlog::get("command")) {
  SetInterruptible(true);
  AddSequential(new shooter::SetShooter(kCloseShotSpeed, kCloseShotElevation));
  AddSequential(new StartHopper(kCloseShotHopperVoltage));
}

/**
 * Called if shooting sequence cancelled by button or bad solution.
 */
void StartCloseShot::Interrupted() {
  logger_->info("StartCloseShot shooting sequence cancelled");
  End();
}

//
// StopShooting
//
StopShooting::StopShooting() : frc::CommandGroup("StopShooting") {
  AddParallel(new deadeye::ShooterLED(false));
  AddParallel(new StopHopper());
  AddParallel(new StopIntake());
  AddSequential(new StopShooterWheel());
}
