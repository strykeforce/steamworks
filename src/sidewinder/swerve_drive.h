#pragma once

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "talon_map.h"

namespace sidewinder {

class SwerveDrive : public ::Subsystem {
 private:
  const std::shared_ptr<spdlog::logger> logger_;
  const TalonMap* map_;
  void SetEncoderZero(const std::shared_ptr<cpptoml::table> config);

 public:
  SwerveDrive(const std::shared_ptr<cpptoml::table> config,
              const TalonMap* map);
  void InitDefaultCommand() override;
  void ZeroAzimuth();
  void CrabDrive(float forward, float azimuth);
};
} /* sidewinder */
