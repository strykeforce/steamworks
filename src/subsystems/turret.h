#pragma once

#include <memory>

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

namespace avenger {

class Turret : public ::Subsystem {
 private:
  const std::shared_ptr<spdlog::logger> logger_;

 public:
  Turret(const std::shared_ptr<cpptoml::table> config);
  Turret(const Turret&) = delete;
  Turret& operator=(const Turret&) = delete;
  virtual ~Turret() = default;
};
} /* avenger */
