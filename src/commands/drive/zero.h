#pragma once

#include "WPILib.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace command {
namespace drive {
class Zero : public frc::Command {
 public:
  Zero();

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void End() override;
};

} /* drive */
} /* command */
} /* steamworks */
