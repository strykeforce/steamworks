#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace subsystem {

class Intake : public frc::Subsystem {
 public:
  Intake(const std::shared_ptr<cpptoml::table> config);
  Intake& operator=(Intake&) = delete;
  Intake(Intake&) = delete;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

} /* subsystem */
} /* steamworks */
