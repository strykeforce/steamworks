#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace drive {

class PositionDrive : public frc::Command {
 public:
  PositionDrive(int azimuth);

 protected:
  void Initialize() override;
  bool IsFinished() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  int azimuth_;
};

} /* drive */
} /* command */
} /* steamworks */
