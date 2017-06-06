#pragma once

#include "WPILib.h"
#include "sidewinder/swerve/swerve_drive.h"

namespace avenger {
class AvengerDrive : public sidewinder::swerve::SwerveDrive {
 private:
 public:
  AvengerDrive(const std::shared_ptr<cpptoml::table> config);
  void InitDefaultCommand() override;
};
}  // namespace avenger
