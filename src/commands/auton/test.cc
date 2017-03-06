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
  AddSequential(new Log("PositionDrive out"));
  AddSequential(new drive::PositionDrive(kForwardAzimuth));
  AddSequential(new drive::Drive(kForwardDistance, kForwardAzimuth, 10));

  AddSequential(new Log("GyroAzimuth -90"));
  AddSequential(new drive::PositionAzimuth());
  AddSequential(new drive::GyroAzimuth(-90));

  AddSequential(new Log("PositionDrive into Hopper"));
  AddSequential(new drive::PositionDrive(kForwardAzimuth));
  AddSequential(new drive::Drive(43 * 50.72, kForwardAzimuth, 3));

  // AddSequential(new Log("Wait 1"));
  // AddSequential(new WaitCommand(1));

  AddSequential(new Log("PositionDrive out 6 in."));
  AddSequential(new drive::PositionDrive(0));
  AddSequential(new drive::Drive(15 * 50.72, 0, 10));

  AddSequential(new Log("GyroAzimuth 20"));
  AddSequential(new drive::PositionAzimuth());
  AddSequential(new drive::GyroAzimuth(20));
}
