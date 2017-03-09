#include "test.h"

#include "commands/commands.h"

using namespace steamworks::command::auton;
using namespace steamworks::command;

namespace {
const int kForwardAzimuth = 2048;
const int kStrafeAzimuth = 4096 - 1024;
const int kForwardDistance = static_cast<int>(78 * 50.72);
const int kStrafeDistance = static_cast<int>(49 * 50.72);
}

/**
 * Command group for auton teting
 */
AutonTestSeq::AutonTestSeq() : frc::CommandGroup("AutonTestSeq") {
  // drive on slant to hopper
  AddSequential(new drive::PositionDrive(121.0 / 180.0 * 2048));
  AddSequential(new drive::Drive(92 * 50.72, 121.0 / 180.0 * 2048, 3));

  AddSequential(new WaitCommand(1));

  // move out from wall
  AddSequential(new drive::PositionDrive(0.0 / 180.0 * 2048));
  AddSequential(new drive::Drive(18 * 50.72, 0.0 / 180.0 * 2048, 1));

  // azimuth
  AddSequential(new drive::TimedAzimuth(0.33, 0.8));

  // timed swerve drive
  // AddSequential(new drive::TimedSwerveDrive(-0.55, -0.9, 1.0));

  // pre-azimuth
  // AddSequential(new drive::TimedAzimuth());
  // AddSequential(new drive::TimedAzimuth());

  // AddSequential(new StartShooting());

  // first hopper dump - blue alliance side
  // AddSequential(new Log("PositionDrive out"));
  // AddSequential(new drive::PositionDrive(kForwardAzimuth));
  // AddSequential(new drive::Drive(kForwardDistance, kForwardAzimuth, 10));
  //
  // AddSequential(new Log("GyroAzimuth -90"));
  // AddSequential(new drive::TimedAzimuth());
  // AddSequential(new drive::GyroAzimuth(-90));
  //
  // AddSequential(new Log("PositionDrive into Hopper"));
  // AddSequential(new drive::PositionDrive(kForwardAzimuth));
  // AddSequential(new drive::Drive(47 * 50.72, kForwardAzimuth, 3));
  //
  // // AddSequential(new Log("Wait 1"));
  // // AddSequential(new WaitCommand(1));
  //
  // AddSequential(new Log("PositionDrive out 6 in."));
  // AddSequential(new drive::PositionDrive(0));
  // AddSequential(new drive::Drive(15 * 50.72, 0, 10));
  //
  // AddSequential(new Log("GyroAzimuth 20"));
  // AddSequential(new drive::TimedAzimuth());
  // AddSequential(new drive::GyroAzimuth(20));
  //
  // AddSequential(new StartShooting());
}
