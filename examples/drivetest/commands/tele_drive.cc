#include "tele_drive.h"

#include "WPILib.h"
#include "spdlog/spdlog.h"

#include "oi.h"
#include "robot.h"
#include "subsystems/drive.h"

using namespace sidewinder;

TeleDrive::TeleDrive()
    : ::Command("TeleDrive"),
      logger_(spdlog::stdout_color_st("TeleDrive")),
      print_timer_(std::unique_ptr<::Timer>(new ::Timer())) {
  Requires(Robot::drive);
  logger_->set_level(spdlog::level::debug);
  print_timer_->Start();
}

void TeleDrive::Initialize() {
  logger_->debug("start driving!");
  print_timer_->Reset();
}

void TeleDrive::Execute() {
  float forward = Robot::oi->GetTeleDriveForwardAxis();
  float strafe = Robot::oi->GetTeleDriveStrafeAxis();
  float azimuth = Robot::oi->GetTeleDriveAzimuthAxis();
  Robot::drive->CartesianDrive(forward, strafe, azimuth);
  if (print_timer_->HasPeriodPassed(2.0)) {
    logger_->trace("driving with forward: {}, strafe: {}, azimuth: {}", forward,
                   strafe, azimuth);
  }
}

bool TeleDrive::IsFinished() { return false; }

void TeleDrive::End() {
  logger_->debug("done driving.");
  Robot::drive->CartesianDrive(0.0, 0.0, 0.0);
}

void TeleDrive::Interrupted() {
  logger_->debug("interrupted driving.");
  End();
}
