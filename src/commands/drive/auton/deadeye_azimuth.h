#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace drive {

class DeadeyeAzimuth : public frc::Command {
 public:
  DeadeyeAzimuth(bool offset = false);

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  bool has_offset_;
  double offset_;
  double error_;
  double abs_error_;
  int stable_count_ = 0;
};

} /* drive */
} /* command */
} /* steamworks */
