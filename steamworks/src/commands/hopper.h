#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {
class StartHopper : public frc::InstantCommand {
 public:
  StartHopper();
  StartHopper(double voltage);

 protected:
  void Initialize() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  double voltage_ = 0.0;
};

class StopHopper : public frc::InstantCommand {
 public:
  StopHopper();

 protected:
  void Initialize() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

class ToggleHopper : public frc::InstantCommand {
 public:
  ToggleHopper();

 protected:
  void Initialize() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  StartHopper start_cmd_;
  StopHopper stop_cmd_;
};

class IncrementHopperVoltage : public frc::InstantCommand {
 public:
  IncrementHopperVoltage();

 protected:
  void Initialize() override;
};

class DecrementHopperVoltage : public frc::InstantCommand {
 public:
  DecrementHopperVoltage();

 protected:
  void Initialize() override;
};

} /* command */
} /* steamworks */
