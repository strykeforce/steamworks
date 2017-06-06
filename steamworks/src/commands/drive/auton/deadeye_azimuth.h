#pragma once

#include "config.h"

#ifdef LOG_DEADEYE
#include <fstream>
#endif

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace drive {

class DeadeyeAzimuth : public frc::Command {
 public:
  DeadeyeAzimuth(bool offset = false);

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void Interrupted() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  bool has_offset_;
  bool has_target_;
  double offset_;
  double error_;
  double setpoint_;
  double abs_error_;
  int stable_count_ = 0;

#ifdef LOG_DEADEYE
  frc::Timer timer_;
  double telemetry_start_;
  std::unique_ptr<std::ofstream> telemetry_;
  void InitializeTelemetry();
  void LogTelemetry();
  void EndTelemetry();
#endif
};

} /* drive */
} /* command */
} /* steamworks */
