#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace shooter {
class SetShooter : public frc::Command {
 public:
  SetShooter(int speed, int elevation);
  virtual ~SetShooter() = default;

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void Interrupted() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  int speed_;
  int elevation_;
};
} /* shooter */
} /* command */
} /* steamworks */
