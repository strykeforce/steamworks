#pragma once

#include <WPILib.h>

namespace steamworks {
namespace command {
namespace drive {

class ReadAzimuthCalibration : public frc::InstantCommand {
 public:
  ReadAzimuthCalibration();

 protected:
  void Initialize() override;
};

class WriteAzimuthCalibration : public frc::InstantCommand {
 public:
  WriteAzimuthCalibration();

 protected:
  void Initialize() override;

 private:
  ReadAzimuthCalibration read_cal_;
};

class ZeroWheelAzimuth : public frc::Command {
 public:
  ZeroWheelAzimuth();

 protected:
  void Initialize() override;
  bool IsFinished() override;
  void End() override;
};

} /* drive */
} /* command */
} /* steamworks */
