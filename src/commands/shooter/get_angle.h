#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace shooter {

class GetAngle : public frc::Command {
 public:
  GetAngle();

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  double error_;
  double abs_error_;
  int stable_count_ = 0;
};

} /* drive */
} /* command */
} /* steamworks */
