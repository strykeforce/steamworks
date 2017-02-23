#pragma once

#include "WPILib.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace command {

class DriveTeleOp : public frc::Command {
 public:
  DriveTeleOp();

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;
};

} /* command */
} /* steamworks */
