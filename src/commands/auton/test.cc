#include "test.h"

#include "commands/log.h"

using namespace steamworks::command::auton;

namespace {
const int kForwardAzimuth = 2048;
const int kStrafeAzimuth = 4096 - 1024;
const int kForwardDistance = static_cast<int>(76 * 48.89);
const int kStrafeDistance = static_cast<int>(60 * 48.89);
const int kVelocity = 200;
}

/**
 * Command group for auton teting
 */
AutonTestSeq::AutonTestSeq() : frc::CommandGroup("AutonTestSeq") {
  AddSequential(new Log("Hi"));
}
