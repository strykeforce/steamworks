#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace shooter {
class StartupShot : public frc::Command {
 public:
  StartupShot(int speed, int elevation);
  virtual ~StartupShot() = default;

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  int speed_;
  int elevation_;
};
} /* shooter */
} /* command */
} /* steamworks */
