#pragma once

#include <WPILib.h>
#include <cpptoml/cpptoml.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace subsystem {

class Camera : public frc::Subsystem {
 public:
  Camera(const std::shared_ptr<cpptoml::table> config);
  Camera& operator=(Camera&) = delete;
  Camera(Camera&) = delete;

 private:
  const std::shared_ptr<spdlog::logger> logger_;
};

} /* subsystem */
} /* steamworks */
