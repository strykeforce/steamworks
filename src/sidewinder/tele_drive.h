#pragma once

#include "WPILib.h"
#include "spdlog/spdlog.h"

#include "oi.h"
#include "swerve_drive.h"

namespace sidewinder {

class TeleDrive : public ::Command {
 private:
  const std::shared_ptr<spdlog::logger> logger_;
  const std::unique_ptr<::Timer> print_timer_;
  const OI* oi_;
  SwerveDrive* swerve_drive_;

 public:
  TeleDrive(const OI* oi, SwerveDrive* sd);

  // implements Command
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;
  void Interrupted() override;
};

} /* sidewinder */
