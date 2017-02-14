#pragma once

#include "WPILib.h"
#include "spdlog/spdlog.h"

#include "oi.h"
#include "sidewinder/swerve/swerve_drive.h"

namespace steamworks {
namespace command {

/** DriveTele is a WPI command-based command to manually drive the Sidewinder
 * swerve drive.
 */
class DriveTele : public frc::Command {
 public:
  DriveTele();

 protected:
  void Execute() override;
  bool IsFinished() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

class DriveFieldRelative : public frc::TimedCommand {
 public:
  enum Direction {
    kUp,
    kDown,
    kLeft,
    kRight,
  };
  DriveFieldRelative(Direction direction, double timeout);

 protected:
  void Execute() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  Direction direction_;
};

class DriveClockwise : public frc::TimedCommand {
 public:
  DriveClockwise(double timeout);

 protected:
  void Execute() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

class DriveSquare : public frc::CommandGroup {
 public:
  DriveSquare();

 private:
};

} /* command */
} /* steamworks */
