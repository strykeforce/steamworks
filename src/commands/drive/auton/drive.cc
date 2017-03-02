#include "drive.h"

#include "robot.h"

using namespace steamworks::command::drive;

/**
 * Designated constructor.
 */
Drive::Drive(int distance, int azimuth)
    : frc::Command("Drive"),
      logger_(spdlog::get("command")),
      distance_(distance),
      azimuth_(azimuth) {
  Requires(Robot::drive);
}

/**
 * Initialize with default azimuth of zero.
 */
Drive::Drive(int distance) : Drive(distance, 0) {}

/**
 * Initialize
 */
void Drive::Initialize() {}

/**
 * Execute
 */
void Drive::Execute() {}

/**
 * IsFinished
 */
bool Drive::IsFinished() { return true; }

/**
 * End
 */
void Drive::End() {}
