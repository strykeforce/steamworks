#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace shooter {

class SetWheel : public frc::Command {
 public:
  SetWheel();

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  double speed_;
};

} /* shooter */
} /* command */
} /* steamworks */
