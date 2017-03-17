#include "sequence_04.h"

#include "commands/drive/auton/drive.h"
#include "commands/drive/auton/gyro_azimuth.h"
#include "commands/intake.h"
#include "commands/log.h"
#include "commands/shooter/sequence.h"
#include "commands/shooter/set_shooter.h"

using namespace steamworks::command::auton;
using namespace steamworks::command;

namespace {
const int kPrepareSpeed = 400;
const int kPrepareElevation = 1800;
}

namespace {
const int kForwardAzimuth = 2048;
}

/**
 * Blue move out and shoot 10
 */
Sequence04::Sequence04() : frc::CommandGroup("Sequence04") {
  AddSequential(
      new LogCommand("starting BLUE alliance move out and and shoot"));
  AddParallel(new shooter::SetShooter(kPrepareSpeed, kPrepareElevation));
  // AddSequential(new drive::TimedSwerveDrive(-0.25, 0, 2.2));

  // azimuth
  // AddSequential(new drive::TimedAzimuth(0.53, 0.4));

  // // start shooting
  AddSequential(new StartShooting());
}
