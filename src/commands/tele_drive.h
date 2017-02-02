#pragma once

#include "WPILib.h"
#include "spdlog/spdlog.h"

#include "oi.h"
#include "sidewinder/swerve/swerve_drive.h"

namespace steamworks {

/** TeleDrive is a WPI command-based command to manually drive the Sidewinder
 * swerve drive.
 */
class TeleDrive : public frc::Command {
 private:
  const std::shared_ptr<spdlog::logger> logger_;
  const std::unique_ptr<frc::Timer> print_timer_;

 public:
  /** Construct the TeleDrive.
   */
  TeleDrive();

  // implements Command
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;
  void Interrupted() override;
};

} /* steamworks */
