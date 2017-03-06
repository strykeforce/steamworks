#include "sequence.h"

#include "commands/commands.h"

using namespace steamworks::command;

StartShooting::StartShooting() : frc::CommandGroup("StartShooting") {
  SetInterruptible(true);
  AddParallel(new deadeye::GearLED(true));
  AddParallel(new SetRange());
  AddSequential(new drive::DeadeyeAzimuth());
  AddParallel(new deadeye::GearLED(false));
  AddSequential(new StartHopper());
}

StopShooting::StopShooting() : frc::CommandGroup("StopShooting") {
  SetInterruptible(false);
  AddParallel(new deadeye::GearLED(false));
  AddParallel(new StopHopper());
  AddParallel(new StopShooterWheel());
}
