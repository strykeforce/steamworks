#include "tele_drive.h"

#include "WPILib.h"
#include "spdlog/spdlog.h"

#include "oi.h"
#include "swerve_drive.h"

namespace sidewinder {

TeleDrive::TeleDrive(const OI* oi, SwerveDrive* sd)
    : ::Command("command::TeleDrive"),
      logger_(spdlog::stdout_color_st("TeleDrive")),
      print_timer_(std::unique_ptr<::Timer>(new ::Timer())),
      oi_(oi),
      swerve_drive_(sd) {
  Requires(swerve_drive_);
  logger_->set_level(spdlog::level::debug);
  print_timer_->Start();
}

void TeleDrive::Initialize() {
  logger_->debug("start driving!");
  // swerve_drive_->SetControlMode(subsystem::TankDrive::kVoltageMode);
  print_timer_->Reset();
}

void TeleDrive::Execute() {
  float forward = oi_->GetTeleDriveForwardAxis();
  float strafe = oi_->GetTeleDriveStrafeAxis();
  float azimuth = oi_->GetTeleDriveAzimuthAxis();
  swerve_drive_->Drive(forward, strafe, azimuth);
  if (print_timer_->HasPeriodPassed(2.0)) {
    logger_->debug("driving with forward: {}, strafe: {}, azimuth: {}", forward,
                   strafe, azimuth);
  }
}

bool TeleDrive::IsFinished() { return false; }

void TeleDrive::End() {
  logger_->debug("done driving.");
  swerve_drive_->CrabDrive(0.0, 0.0);
}

void TeleDrive::Interrupted() {
  logger_->debug("interrupted driving.");
  End();
}

} /* sidewinder */
