#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
namespace drive {
class Drive : public frc::Command {
 public:
  Drive(int distance, int azimuth);
  Drive(int distance);
  virtual ~Drive() = default;

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  double distance_;
  int azimuth_ = 0;
  double error_;
  double abs_error_;
  double start_decel_pos_;
  double stable_count_ = 0;
};
} /* drive */
} /* command */
} /* steamworks */
