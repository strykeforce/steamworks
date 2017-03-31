#include "sequence_01.h"

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

/**
 * Blue hopper dump and shoot
 * Gyro offset = -90
 */
Sequence01::Sequence01() : frc::CommandGroup("Sequence01") {
  bool is_practice = RobotMap::IsPracticeRobot();

  std::string msg = is_practice ? "BLUE alliance hopper dump on PRACTICE (01)"
                                : "BLUE alliance hopper dump on COMP (01)";
  AddSequential(new LogCommand(msg));

  drive::DriveConfig dc;
  dc.min_speed = 40;
  dc.max_speed = 400;
  dc.acceleration = 400;
  dc.deacceleration = 10000;
  dc.close_enough = 10 * kTicksPerInch;
  dc.segments.emplace_back(-100, 68 * kTicksPerInch);  // drive out
  dc.segments.emplace_back(180, 36 * kTicksPerInch);   // drive into hopper
  dc.timeout = 2.2;
  AddSequential(new drive::Drive(dc));

  // drive down wall
  dc.deacceleration = 60;
  dc.close_enough = 25;
  dc.timeout = 1.0;
  dc.segments.clear();
  dc.segments.emplace_back(112, 12 * kTicksPerInch);  // strafe down wall
  AddSequential(new drive::Drive(dc));

  // wait for balls
  AddSequential(new WaitCommand(1.0));
  AddParallel(new shooter::SetShooter(kPrepareSpeed, kPrepareElevation));

  // drive out from hopper
  dc.timeout = -1.0;
  dc.segments.clear();
  dc.segments.emplace_back(0, 16 * kTicksPerInch);  // drive out from hopper
  AddSequential(new drive::Drive(dc));

  // pre-azimuth towards boiler
  AddSequential(new drive::GyroAzimuth(100));

  // // // start shooting
  AddParallel(new StartShooting());
  AddSequential(new WaitCommand(4.0));
  AddParallel(new StartIntake());
  AddParallel(new gear::StageGear());
}
