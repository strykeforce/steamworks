#include "sequence_02.h"

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
const int kPrepareElevation = 1300;
}

/**
 * Red hopper dump and shoot
 * Gyro offset = 90
 */
Sequence02::Sequence02() : frc::CommandGroup("Sequence02") {
  bool is_practice = RobotMap::IsPracticeRobot();

  std::string msg = is_practice ? "RED alliance hopper dump on PRACTICE (02)"
                                : "RED alliance hopper dump on COMP (02)";
  AddSequential(new LogCommand(msg));

  drive::DriveConfig dc;
  dc.min_speed = 40;
  dc.max_speed = 400;
  dc.acceleration = 400;
  dc.deacceleration = 10000;
  dc.close_enough = 10 * kTicksPerInch;
  dc.segments.emplace_back(100, 70 * kTicksPerInch);  // drive out
  dc.segments.emplace_back(180, 40 * kTicksPerInch);  // drive into hopper
  dc.timeout = 2.2;
  AddSequential(new drive::Drive(dc));

  // drive down wall
  dc.deacceleration = 60;
  dc.close_enough = 25;
  dc.timeout = 1.2;
  dc.segments.clear();
  dc.segments.emplace_back(-112, 20 * kTicksPerInch);  // strafe down wall
  AddSequential(new drive::Drive(dc));

  // wait for balls
  AddSequential(new WaitCommand(1.0));
  AddSequential(new shooter::SetShooter(kPrepareSpeed, kPrepareElevation));

  // drive out from hopper
  dc.timeout = -1.0;
  dc.segments.clear();
  dc.segments.emplace_back(0, 16 * kTicksPerInch);  // drive out from hopper
  AddSequential(new drive::Drive(dc));

  // pre-azimuth towards boiler
  AddSequential(new drive::GyroAzimuth(-90));

  // // // start shooting
  AddParallel(new StartShooting());
  AddSequential(new WaitCommand(4.0));
  AddParallel(new StartIntake());
  AddParallel(new gear::StageGear());
}
