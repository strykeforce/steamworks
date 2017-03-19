#include "sequence_01.h"

#include "commands/drive/auton/drive.h"
#include "commands/drive/auton/gyro_azimuth.h"
#include "commands/intake.h"
#include "commands/log.h"
#include "commands/shooter/sequence.h"
#include "commands/shooter/set_shooter.h"

using namespace steamworks::command::auton;
using namespace steamworks::command;

namespace {
const double kTicksPerInch = 50.72;
const int kPrepareSpeed = 400;
// const int kPrepareElevation = 1800;
const int kPrepareElevation = 1000;
}

/**
 * Blue hopper dump and shoot
 */
Sequence01::Sequence01() : frc::CommandGroup("Sequence01") {
  AddSequential(new LogCommand("starting BLUE alliance hopper dump and shoot"));

  drive::DriveConfig dconfig;
  dconfig.angle = -105;
  dconfig.distance = 79 * kTicksPerInch;
  dconfig.min_speed = 40;
  dconfig.max_speed = 300;
  dconfig.acceleration = 400;
  dconfig.deacceleration = 60;
  dconfig.close_enough = 25;

  // drive out
  AddSequential(new drive::Drive(dconfig));

  // drive into hopper
  dconfig.angle = 180;
  dconfig.distance = 26 * kTicksPerInch;
  dconfig.deacceleration = 10000;
  dconfig.close_enough = 10 * kTicksPerInch;
  dconfig.timeout = 2.0;
  AddSequential(new drive::Drive(dconfig));

  // drive down wall
  dconfig.angle = 100;
  dconfig.distance = 9 * kTicksPerInch;
  dconfig.deacceleration = 60;
  dconfig.close_enough = 25;
  dconfig.timeout = 1.0;
  AddSequential(new drive::Drive(dconfig));

  // wait for balls
  AddSequential(new WaitCommand(1.0));

  // drive out from hopper
  AddParallel(new shooter::SetShooter(kPrepareSpeed, kPrepareElevation));
  dconfig.angle = 0;
  dconfig.distance = 16 * kTicksPerInch;
  dconfig.timeout = -1.0;
  AddSequential(new drive::Drive(dconfig));

  // pre-azimuth towards boiler
  AddSequential(new drive::GyroAzimuth(100));

  // // // start shooting
  AddParallel(new StartShooting());
  AddSequential(new WaitCommand(4.0));
  AddSequential(new StartIntake());
}
