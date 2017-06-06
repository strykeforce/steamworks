#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace drive {

class ZeroGyro : public frc::InstantCommand {
 public:
  ZeroGyro();

 protected:
  void Initialize() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

} /* drive */
} /* command */
} /* steamworks */
