#pragma once

#include "WPILib.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace command {

class DriveAzimuth : public frc::Command, public PIDOutput {
 public:
  DriveAzimuth(double angle);

  void PIDWrite(double output) override;  // PIDOutput

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  std::unique_ptr<PIDController> controller_;
  double angle_, azimuth_rate_ = 0.0;
};

} /* command */
} /* steamworks */
