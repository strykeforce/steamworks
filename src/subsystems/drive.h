#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "sidewinder/swerve/swerve_drive.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace subsystem {

class SwerveDrive : public sidewinder::swerve::SwerveDrive {
 public:
  SwerveDrive(const std::shared_ptr<cpptoml::table> config);
  void InitDefaultCommand() override;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

} /* subsystem */
} /* steamworks */
