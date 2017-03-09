#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace drive {

/**
 * Aligns wheel position for auton rotation.
 */
class TimedSwerveDrive : public frc::Command {
 public:
  TimedSwerveDrive(double forward, double strafe, double timeout);

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void End() override;

 private:
  // const std::shared_ptr<spdlog::logger> logger_;
  double forward_;
  double strafe_;
};

} /* drive */
} /* command */
} /* steamworks */
