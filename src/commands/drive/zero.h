#pragma once

#include "WPILib.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace command {

class DriveZero : public frc::Command {
 public:
  DriveZero();

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void End() override;
};

} /* command */
} /* steamworks */
