#pragma once

#include <WPILib.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace command {

//
// FinishClimb
//
class FinishClimb : public frc::InstantCommand {
 public:
  FinishClimb();

 protected:
  void Initialize();

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

//
// StartClimb
//
class StartClimb : public frc::Command {
 public:
  StartClimb();

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void Interrupted() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  FinishClimb finish_;
};

//
// CaptureRope
//
class CaptureRope : public frc::Command {
 public:
  CaptureRope();

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void Interrupted() override;
  void End() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  StartClimb climb_;
};

class StopClimb : public frc::InstantCommand {
 public:
  StopClimb();

 protected:
  void Initialize() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};
} /* command */
} /* steamworks */
