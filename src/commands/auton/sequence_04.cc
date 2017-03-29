#include "sequence_04.h"

#include "commands/drive/auton/drive.h"
#include "commands/drive/auton/gyro_azimuth.h"
#include "commands/gear/sequence.h"
#include "commands/intake.h"
#include "commands/log.h"
#include "commands/shooter/sequence.h"
#include "commands/shooter/set_shooter.h"
#include "robot_map.h"

using namespace steamworks::command::auton;
using namespace steamworks::command;

namespace {
const double kTicksPerInch = 50.72;
const int kPrepareSpeed = 440;
const int kPrepareElevation = 2000;
}

namespace {
const int kForwardAzimuth = 2048;
}

/**
 * Blue move out and shoot 10
 * Gyro offset = 0
 */
Sequence04::Sequence04() : frc::CommandGroup("Sequence04") {
  bool is_practice = RobotMap::IsPracticeRobot();

  std::string msg = is_practice ? "BLUE alliance out and shoot on PRACTICE (04)"
                                : "BLUE alliance out and shoot on COMP (04)";
  AddSequential(new LogCommand(msg));

  AddSequential(
      new LogCommand("starting BLUE alliance move out and shoot (04)"));

  AddParallel(new shooter::SetShooter(kPrepareSpeed, kPrepareElevation));

  drive::DriveConfig dc;
  dc.min_speed = 40;
  dc.max_speed = 400;
  dc.acceleration = 400;
  dc.deacceleration = 10000;
  dc.close_enough = 10 * kTicksPerInch;
  dc.segments.emplace_back(-180, 68 * kTicksPerInch);  // drive out
  AddSequential(new drive::Drive(dc));

  // pre-azimuth towards boiler
  AddSequential(new drive::GyroAzimuth(40));

  // // // start shooting
  AddParallel(new StartShooting());
  AddSequential(new WaitCommand(4.0));
  AddSequential(new gear::StageGear());
}
