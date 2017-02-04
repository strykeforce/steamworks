#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace subsystem {

class Shooter : public frc::Subsystem {
 public:
  Shooter(const std::shared_ptr<cpptoml::table> config);
  Shooter& operator=(Shooter&) = delete;
  Shooter(Shooter&) = delete;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

} /* subsystem */
} /* steamworks */
