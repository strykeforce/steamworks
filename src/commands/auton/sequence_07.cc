#include "sequence_07.h"

#include "commands/deadeye/gear_led.h"
#include "commands/deadeye/mode.h"
#include "commands/drive/auton/drive.h"
#include "commands/drive/auton/gyro_azimuth.h"
#include "commands/gear/auton/place_gear.h"
#include "commands/gear/release.h"
#include "commands/gear/sequence.h"
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
 * Red move out and shoot 10
 */
Sequence07::Sequence07() : frc::CommandGroup("Sequence07") {
  AddSequential(new deadeye::EnableCamera(deadeye::EnableCamera::Mode::gear));

  bool is_practice = RobotMap::IsPracticeRobot();

  std::string msg = is_practice ? "BLUE alliance left gear on PRACTICE (07)"
                                : "BLUE alliance left gear on COMP (07)";
  AddSequential(new LogCommand(msg));

  // stage the gear
  AddParallel(new gear::StageGear());

  // drive out
  drive::DriveConfig dc;
  dc.min_speed = 40;
  dc.max_speed = 200;
  dc.acceleration = 200;
  dc.deacceleration = 60;
  dc.close_enough = 25;
  dc.segments.emplace_back(-180, 96 * kTicksPerInch);  // drive out
  AddSequential(new drive::Drive(dc));

  // square up
  AddSequential(new drive::GyroAzimuth(60));

  // strafe to target
  AddSequential(new deadeye::GearLED(true));
  AddSequential(new gear::PlaceGear(gear::Lift::left));

  // push to wall
  dc.max_speed = 100;
  dc.segments.clear();
  dc.segments.emplace_back(-120, 16 * kTicksPerInch);
  AddSequential(new drive::Drive(dc));

  // release gear
  AddParallel(new gear::ReleaseGear());
#if 0

  // back off
  dc.max_speed = 200;
  dc.segments.clear();
  dc.segments.emplace_back(0, 14 * kTicksPerInch);
  AddSequential(new drive::Drive(dc));

  // azimuth to boiler
  AddSequential(new drive::GyroAzimuth(75));
#endif
}
