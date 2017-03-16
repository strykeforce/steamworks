#include "sequence_01.h"

#include "commands/commands.h"

using namespace steamworks::command::auton;
using namespace steamworks::command;

namespace {
const int kForwardAzimuth = 2048;
const int kStrafeAzimuth = 4096 - 1024;
const int kForwardDistance = static_cast<int>(78 * 50.72);
const int kStrafeDistance = static_cast<int>(49 * 50.72);
const int kPrepareSpeed = 400;
const int kPrepareElevation = 1800;
}

/**
 * Blue hopper dump and shoot
 */
Sequence01::Sequence01() : frc::CommandGroup("Sequence01") {
  AddSequential(new LogCommand("starting BLUE alliance hopper dump and shoot"));

  // drive on slant to hopper
  // -0.5, 0.95
  // -0.55, 0.90
  // -0.6, 0.85
  // -0.65, -0.8
  // -0.62, 0.82
  // -0.64, 0.82
  AddSequential(new drive::TimedSwerveDrive(0.65 / 2.0, 0.8 / 2.0, 0.15));
  AddSequential(new drive::TimedSwerveDrive(-0.65 / 2.0, -0.8 / 2.0, 1.85));

  AddParallel(new shooter::SetShooter(kPrepareSpeed, kPrepareElevation));
  AddSequential(new WaitCommand(2.0));

  // move out from wall
  AddSequential(new drive::TimedSwerveDrive(0.7, 0, 0.6));

  // azimuth
  AddSequential(new drive::TimedAzimuth(0.4, 0.8));
  //
  // // start shooting
  AddParallel(new StartShooting());
  AddSequential(new WaitCommand(4.0));
  AddSequential(new StartIntake());
}
