#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

namespace steamworks {
namespace subsystem {

class GearLoader : public frc::Subsystem {
 public:
  GearLoader(const std::shared_ptr<cpptoml::table> config);
  GearLoader& operator=(GearLoader&) = delete;
  GearLoader(GearLoader&) = delete;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

} /* subsystem */
} /* steamworks */
