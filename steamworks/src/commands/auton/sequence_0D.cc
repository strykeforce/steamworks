#include "sequence_0D.h"

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
const int kPrepareElevation = 4400;
}  // namespace

/**
 * Blue right gear, run down field
 */
Sequence0D::Sequence0D() : frc::CommandGroup("Sequence0D") {
  AddSequential(new deadeye::EnableCamera(deadeye::EnableCamera::Mode::gear));

  bool is_practice = RobotMap::IsPracticeRobot();

  std::string msg =
      is_practice ? "BLUE alliance right gear, dump hoppers on PRACTICE (0D)"
                  : "BLUE alliance right gear, dump hoppers on COMP (0D)";
  AddSequential(new LogCommand(msg));

  // stage the gear
  AddParallel(new gear::StageGear());
  AddParallel(new deadeye::GearLED(true));

  // drive out
  drive::DriveConfig dc;
  dc.min_speed = 40;
  dc.max_speed = 400;
  dc.acceleration = 200;
  dc.deacceleration = 60;
  dc.close_enough = 25;

  dc.segments.emplace_back(180, 96 * kTicksPerInch);  // drive out
  AddSequential(new drive::Drive(dc));

  // rotate to face gear Lift
  AddSequential(new drive::GyroAzimuth(-60));

  // approach target
  AddSequential(new gear::PlaceGear(gear::Lift::right));

  // release gear
  AddParallel(new gear::ReleaseGear());
  AddParallel(new deadeye::GearLED(false));

  //  back off
  dc.max_speed = 400;
  dc.segments.clear();
  dc.segments.emplace_back(-60, 24 * kTicksPerInch);
  AddSequential(new drive::Drive(dc));

  // pivot into drive alignment
  AddSequential(new drive::GyroAzimuth(0));

  // drive down field
  dc.max_speed = 600;
  dc.segments.clear();
  dc.segments.emplace_back(-140, 127 * kTicksPerInch);
  dc.segments.emplace_back(150, 16 * kTicksPerInch);
  dc.segments.emplace_back(180, 16 * 12 * kTicksPerInch);
  AddSequential(new drive::Drive(dc));

  // pivot and back into hopper
  AddSequential(new drive::GyroAzimuth(90));

  dc.segments.clear();
  dc.max_speed = 400;
  dc.segments.emplace_back(-90, 24 * kTicksPerInch);
  AddSequential(new drive::Drive(dc));
}
