#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

#include "finish_climb.h"

namespace steamworks {
namespace command {
namespace climber {
class StartClimb : public frc::Command {
 public:
  StartClimb();

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void Interrupted() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  FinishClimb finish_;
};
} /* climber */
} /* command */
} /* steamworks */
