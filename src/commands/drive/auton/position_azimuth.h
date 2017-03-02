#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace drive {

/**
 * Aligns wheel position for auton rotation.
 */
class PositionAzimuth : public frc::Command {
 public:
  PositionAzimuth();

 protected:
  void Initialize() override;
  bool IsFinished() override;
};

} /* drive */
} /* command */
} /* steamworks */
