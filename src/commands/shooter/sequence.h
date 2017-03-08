#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {

class TimedTest : public frc::TimedCommand {
 public:
  TimedTest(std::string name, double timeout);

 protected:
  void Initialize() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  std::string name_;
};

class StartShooting : public frc::CommandGroup {
 public:
  StartShooting();

 protected:
  void Interrupted() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

class StopShooting : public frc::CommandGroup {
 public:
  StopShooting();
};

} /* command */
} /* steamworks */
