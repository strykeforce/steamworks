#pragma once

#include "WPILib.h"
#include "spdlog/spdlog.h"

#include "commands/log.h"

namespace steamworks {
namespace command {

class DriveAutonomous : public frc::Command {
 public:
  DriveAutonomous(double forward, double strafe, double azimuth, int distance);

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  int initial_pos_, distance_;
  double forward_, strafe_, azimuth_;
};

class AutonTestSeq : public frc::CommandGroup {
 public:
  AutonTestSeq();
};

} /* command */
} /* steamworks */
