#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace climber {
class FinishClimb : public frc::InstantCommand {
 public:
  FinishClimb();

 protected:
  void Initialize();

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};
} /* climber */
} /* command */
} /* steamworks */
