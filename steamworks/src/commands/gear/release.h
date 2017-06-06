#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace gear {
class ReleaseGear : public frc::Command {
 public:
  ReleaseGear();

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void End() override;
  void Interrupted() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  int stable_count_;
  int pivot_pos_;
};
} /* gear */
} /* command */
} /* steamworks */
