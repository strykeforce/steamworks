#include "tele_op.h"

#include "oi.h"
#include "robot.h"
#include "subsystems/drive.h"

using namespace steamworks::command::drive;

/**
 * DriveTele is a WPI command-based command to manually drive the Sidewinder
 * swerve drive.
 */
DriveTeleOp::DriveTeleOp() : frc::Command("DriveTeleOp") {
  Requires(Robot::drive);
}

/**
 * Ensure the drive Talons are in the correct mode for TeleOp.
 */
void DriveTeleOp::Initialize() { Robot::drive->SetTeleOpMode(); }

void DriveTeleOp::Execute() {
  double forward = Robot::oi->GetTeleDriveForwardAxis();
  double strafe = Robot::oi->GetTeleDriveStrafeAxis();
  double azimuth = Robot::oi->GetTeleDriveAzimuthAxis();
  Robot::drive->Drive(forward, strafe, azimuth);
}

bool DriveTeleOp::IsFinished() { return false; }

void DriveTeleOp::End() { Robot::drive->Drive(0.0, 0.0, 0.0); }
