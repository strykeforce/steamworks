#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace drive {
class Drive : public frc::Command {
 public:
  // this is called by other constructors
  Drive(double angle, double distance, double timeout);
  Drive(double angle, double distance);
  virtual ~Drive() = default;

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  double angle_;
  double distance_;
  double forward_factor_;
  double strafe_factor_;

  double error_;
  double abs_error_;
  double start_decel_pos_;

  int stable_count_ = 0;
  int zero_count_ = 0;
};
} /* drive */
} /* command */
} /* steamworks */
