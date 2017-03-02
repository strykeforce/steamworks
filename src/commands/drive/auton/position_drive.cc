#include "position_drive.h"

#include "robot.h"

using namespace steamworks::command::drive;

namespace {
// TODO: azimuth tolerance can be tightened up
const int kAzimuthTolerance = 57;  // about 5 degrees
}

/**
 * Set wheel direction for next segment in autonomous.
 *
 * @param azimuth wheel position in encoder ticks, -2048 to 2048
 */
PositionDrive::PositionDrive(int azimuth)
    : frc::Command("PositionDrive"),
      logger_(spdlog::get("command")),
      azimuth_(azimuth) {
  Requires(Robot::drive);
}

/**
 * Sends the command to azimuth the wheels to the desired position.
 */
void PositionDrive::Initialize() {
  logger_->info("positioning wheels to {}", azimuth_);
  Robot::drive->CrabDriveAutonomous(0, azimuth_);
}

/**
 * Waits for wheel azimuths to be in position.
 */
bool PositionDrive::IsFinished() {
  logger_->debug("position wheels error {}",
                 std::abs(azimuth_ - Robot::drive->GetAzimuth()));
  return std::abs(azimuth_ - Robot::drive->GetAzimuth()) < kAzimuthTolerance;
}

/**
 * Reset the drive encoders to zero.
 */
void PositionDrive::End() { Robot::drive->ZeroDistance(); }
