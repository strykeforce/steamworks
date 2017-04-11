#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

#include "finish_climb.h"

namespace steamworks {
namespace command {
namespace climber {
class StopClimb : public frc::InstantCommand {
 public:
  StopClimb();

 protected:
  void Initialize() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};
} /* climber */
} /* command */
} /* steamworks */
