#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace drive {

/**
 * Aligns wheel position for auton rotation.
 */
class TimedDrive : public frc::Command {
 public:
  TimedDrive();

 protected:
  void Initialize() override;
  bool IsFinished() override;

 private:
  // const std::shared_ptr<spdlog::logger> logger_;
  // int distance_;
  // int initial_;
};

} /* drive */
} /* command */
} /* steamworks */
