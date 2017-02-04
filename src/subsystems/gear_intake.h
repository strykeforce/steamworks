#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace subsystem {

class GearIntake : public frc::Subsystem {
 public:
  GearIntake(const std::shared_ptr<cpptoml::table> config);
  GearIntake& operator=(GearIntake&) = delete;
  GearIntake(GearIntake&) = delete;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

} /* subsystem */
} /* steamworks */
