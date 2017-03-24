#pragma once

#include <WPILib.h>
#include <cpptoml/cpptoml.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace subsystem {

class Climber : public frc::Subsystem {
 public:
  Climber(const std::shared_ptr<cpptoml::table> config);
  Climber& operator=(Climber&) = delete;
  Climber(Climber&) = delete;

  void SetCaptureModeEnabled(bool enabled);
  void SetFinishModeEnabled(bool enabled);
  void SetRatchetModeEnabled(bool enabled);
  void StartCapture();
  void StartClimb();
  void StartFinish();
  void Stop();

  bool IsRunning();
  bool IsCaptured();
  bool ShouldClimbFinish();
  bool IsClimbFinished();

  void ZeroPosition();
  int GetPosition();

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  double capture_voltage_ = 0.0;
  double climb_voltage_ = 0.0;
  double finish_voltage_ = 0.0;
  double ratchet_voltage_ = 0.0;
  double capture_current_ = 0.0;
  double finish_current_ = 0.0;
  bool is_running_ = false;
  bool is_zeroed_ = false;
  bool is_ratchet_ = false;
  int climb_distance_ = 0;
  int zero_count_ = 0;

  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
};

} /* subsystem */
} /* steamworks */
