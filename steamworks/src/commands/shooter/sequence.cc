#include "sequence.h"

#include <Commands/ConditionalCommand.h>

#include "commands/deadeye/mode.h"
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
const int kPrepareSpeed = 440;
const int kPrepareElevation = 2700;
const int kCloseShotSpeed = 425;
const int kCloseShotElevation = 0;
const double kCloseShotHopperVoltage = 4.0;
}

StartShooting::StartShooting()
    : frc::CommandGroup("StartShooting"), logger_(spdlog::get("command")) {
  SetInterruptible(true);

  AddSequential(new deadeye::EnableCamera(deadeye::EnableCamera::Mode::boiler));
  AddSequential(new deadeye::ShooterLED(true));

  // SetShooter checks to see if wheel already has speed commanded
  AddSequential(new shooter::SetShooter(kPrepareSpeed, kPrepareElevation));

  AddParallel(new shooter::GetAngle());
  AddParallel(new drive::DeadeyeAzimuth());
  AddSequential(new frc::WaitForChildren(10));

  AddParallel(new shooter::SetElevation());
  AddParallel(new shooter::SetWheel());
  AddSequential(new frc::WaitForChildren(10));

  AddSequential(new StartHopper());

  AddSequential(new deadeye::ShooterLED(false));
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
  AddSequential(new WaitCommand(1.5));
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
  AddParallel(new StopShooterWheel());
}
