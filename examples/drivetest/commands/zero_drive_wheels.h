#pragma once

#include "WPILib.h"

namespace sidewinder {

/** ZeroDriveWheels is a WPI command-based command to zero wheels.
 */
class ZeroDriveWheels : public frc::Command {
 private:
 public:
  ZeroDriveWheels();

  // implements Command
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;
  void Interrupted() override;
};

} /* sidewinder */
