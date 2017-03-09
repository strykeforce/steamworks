#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace drive {

/**
 * Aligns wheel position for auton rotation.
 */
class TimedAzimuth : public frc::Command {
 public:
  TimedAzimuth(double speed, double timeout);

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void End() override;

 private:
  // const std::shared_ptr<spdlog::logger> logger_;
  double speed_;
};

} /* drive */
} /* command */
} /* steamworks */
