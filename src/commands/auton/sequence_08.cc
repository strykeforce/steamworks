#include "sequence_08.h"

#include "commands/deadeye/gear_led.h"
#include "commands/deadeye/mode.h"
#include "commands/drive/auton/drive.h"
#include "commands/drive/auton/gyro_azimuth.h"
#include "commands/gear/auton/place_gear.h"
#include "commands/gear/release.h"
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
const int kPrepareSpeed = 500;
const int kPrepareElevation = 3400;
}

/**
 * Red move out and shoot 10
 */
Sequence08::Sequence08() : frc::CommandGroup("Sequence08") {
  AddSequential(new deadeye::EnableCamera(deadeye::EnableCamera::Mode::gear));

  bool is_practice = RobotMap::IsPracticeRobot();

  std::string msg = is_practice ? "BLUE alliance left gear on PRACTICE (08)"
                                : "BLUE alliance left gear on COMP (08)";
  AddSequential(new LogCommand(msg));

  // stage the gear
  AddParallel(new gear::StageGear());
  AddParallel(new deadeye::GearLED(true));

  // drive out
  drive::DriveConfig dc;
  dc.min_speed = 40;
  dc.max_speed = 300;
  dc.acceleration = 200;
  dc.deacceleration = 40;
  dc.close_enough = 25;
  dc.segments.emplace_back(180, 47 * kTicksPerInch);  // drive out
  dc.segments.emplace_back(170, 47 * kTicksPerInch);  // drive out
  AddSequential(new drive::Drive(dc));

  // rotate to face gear Lift
  AddSequential(new drive::GyroAzimuth(60));

  // approach target
  AddSequential(new gear::PlaceGear(gear::Lift::left));

  // release gear
  AddParallel(new gear::ReleaseGear());
  AddParallel(new deadeye::GearLED(false));

  // spin up shooter
  AddParallel(new shooter::SetShooter(kPrepareSpeed, kPrepareElevation));

  //  back off
  dc.max_speed = 200;
  dc.segments.clear();
  dc.segments.emplace_back(60, 30 * kTicksPerInch);
  AddSequential(new drive::Drive(dc));

  // pivot back towards boiler
  AddSequential(new drive::GyroAzimuth(40));

  // and shoot
  AddSequential(new StartShooting());
}
