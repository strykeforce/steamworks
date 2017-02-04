#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace subsystem {

class Climber : public frc::Subsystem {
 public:
  Climber(const std::shared_ptr<cpptoml::table> config);
  Climber& operator=(Climber&) = delete;
  Climber(Climber&) = delete;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

} /* subsystem */
} /* steamworks */
