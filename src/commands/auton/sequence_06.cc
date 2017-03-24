#include "sequence_06.h"

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
Sequence06::Sequence06() : frc::CommandGroup("Sequence06") {
  AddSequential(new LogCommand("starting BLUE alliance center gear"));
  AddSequential(new deadeye::EnableCamera(deadeye::EnableCamera::Mode::gear));

  // stage the gear
  AddParallel(new gear::StageGear());

  // drive out
  drive::DriveConfig dc;
  dc.min_speed = 40;
  dc.max_speed = 200;
  dc.acceleration = 200;
  dc.deacceleration = 60;
  dc.close_enough = 25;
  dc.segments.emplace_back(-180, 62 * kTicksPerInch);  // drive out
  AddSequential(new drive::Drive(dc));

  // square up
  AddSequential(new drive::GyroAzimuth(0));

  // strafe to target
  AddSequential(new deadeye::GearLED(true));
  AddSequential(new gear::PlaceGear(gear::Lift::center));

  // push to wall
  dc.max_speed = 100;
  dc.segments.clear();
  dc.segments.emplace_back(-180, 14 * kTicksPerInch);
  AddSequential(new drive::Drive(dc));

  // release gear
  AddParallel(new gear::ReleaseGear());
  AddParallel(new deadeye::GearLED(false));

  // back off
  dc.max_speed = 200;
  dc.segments.clear();
  dc.segments.emplace_back(0, 14 * kTicksPerInch);
  AddSequential(new drive::Drive(dc));

  // azimuth to boiler
  AddSequential(new drive::GyroAzimuth(75));

  // and shoot
  AddParallel(new StartShooting());
}
