#pragma once

#include "WPILib.h"
#include "spdlog/spdlog.h"

#include "commands/log.h"

namespace steamworks {
namespace command {

class PositionWheels : public frc::Command {
 public:
  PositionWheels(int azimuth);

 protected:
  void Initialize() override;
  bool IsFinished() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  int azimuth_;
};

class DriveAutonomous : public frc::Command {
 public:
  DriveAutonomous(int velocity, int azimuth, int distance);

 protected:
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  int initial_pos_ = 0;
  int velocity_;
  int azimuth_;
  int distance_;
};

class DriveProfile : public frc::Command {
 public:
  DriveProfile(int distance);

 protected:
  void Initialize() override;
  bool IsFinished() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  int distance_;
};

class AutonTestSeq : public frc::CommandGroup {
 public:
  AutonTestSeq();

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

} /* command */
} /* steamworks */
