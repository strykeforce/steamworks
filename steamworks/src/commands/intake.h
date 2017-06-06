#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
class StartIntake : public frc::Command {
 public:
  StartIntake();
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;

 protected:
 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

class StopIntake : public frc::Command {
 public:
  StopIntake();
  void Initialize() override;
  bool IsFinished() override;

 protected:
 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

class ClearIntake : public frc::Command {
 public:
  ClearIntake();

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

} /* command */
} /* steamworks */
