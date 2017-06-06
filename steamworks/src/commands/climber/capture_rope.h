#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

#include "start_climb.h"

namespace steamworks {
namespace command {
namespace climber {
class CaptureRope : public frc::Command {
 public:
  CaptureRope();

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void Interrupted() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  StartClimb climb_;
};
} /* climber */
} /* command */
} /* steamworks */
