#pragma once

#include "WPILib.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace command {

class DriveAzimuth : public frc::Command {
 public:
  DriveAzimuth(float target);

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  float target_;
  float error_;
  float abs_error_;
  int stable_count_ = 0;
};

class PositionAzimuth : public frc::Command {
 public:
  PositionAzimuth();

 protected:
  void Initialize() override;
  bool IsFinished() override;
};

} /* command */
} /* steamworks */
