#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace drive {

class GyroAzimuth : public frc::Command {
 public:
  GyroAzimuth(float target);

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  float target_;
  float error_;
  float abs_error_;
  int stable_count_ = 0;
};

} /* drive */
} /* command */
} /* steamworks */
