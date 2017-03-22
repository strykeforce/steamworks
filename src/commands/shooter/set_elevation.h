#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace shooter {

class SetElevation : public frc::Command {
 public:
  SetElevation();

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  double elevation_;
};

} /* shooter */
} /* command */
} /* steamworks */
