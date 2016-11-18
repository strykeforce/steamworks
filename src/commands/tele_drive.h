#pragma once

#include "WPILib.h"
#include "spdlog/spdlog.h"

#include "oi.h"
#include "swerve/swerve_drive.h"

namespace sidewinder {

/** TeleDrive is a WPI command-based command to manually drive the Sidewinder
 * swerve drive.
 */
class TeleDrive : public ::Command {
 private:
  const std::shared_ptr<spdlog::logger> logger_;
  const std::unique_ptr<::Timer> print_timer_;

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

} /* sidewinder */
