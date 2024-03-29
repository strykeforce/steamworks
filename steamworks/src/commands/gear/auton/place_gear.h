#pragma once

#include "config.h"

#ifdef LOG_PLACEGEAR
#include <fstream>
#endif

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace motion {
class AzimuthMotion;
} /* motion */

namespace command {
namespace gear {

enum class Lift { left, center, right };

class PlaceGear : public frc::Command {
 public:
  PlaceGear(Lift position);
  virtual ~PlaceGear() = default;

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;

  double distance_;
  // double drive_error_;
  // double drive_abs_error_;

  double strafe_error_;
  double strafe_abs_error_;

  double azimuth_angle_;
  double azimuth_error_;
  double azimuth_abs_error_;

  double drive_setpoint_;
  double strafe_setpoint_;
  double azimuth_setpoint_;

  int stable_count_;
  int zero_count_;

  bool is_delay_done_ = false;
  bool is_cruising_ = false;
  bool is_initialized_ = false;

  bool DelayForPositionZero();
  bool CalculateDistance();

  double CalculateDriveSetpoint();
  double CalculateStrafeSetpoint();
  double CalculateAzimuthSetpoint();

#ifdef LOG_PLACEGEAR
  frc::Timer timer_;
  double telemetry_start_;
  std::unique_ptr<std::ofstream> telemetry_;
  void InitializeTelemetry();
  void LogTelemetry();
  void EndTelemetry();
#endif
};

} /* gear */
} /* command */
} /* steamworks */
