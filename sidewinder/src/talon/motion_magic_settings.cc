#include "motion_magic_settings.h"

using namespace sidewinder::talon;
using namespace std;

MotionMagicSettings::MotionMagicSettings(
    const std::shared_ptr<cpptoml::table> config)
    : PIDSettings(config) {
  auto d_opt = config->get_as<double>("cruise_velocity");
  if (!d_opt) {
    throw invalid_argument("TALON cruise_velocity setting missing");
  }
  cruise_velocity_ = *d_opt;

  d_opt = config->get_as<double>("acceleration");
  if (!d_opt) {
    throw invalid_argument("TALON acceleration setting missing");
  }
  acceleration_ = *d_opt;
}

void MotionMagicSettings::SetMode(::CANTalon* talon) const {
  assert(talon);
  talon->SetTalonControlMode(::CANTalon::kMotionMagicMode);
  PIDSettings::SetMode(talon);
  talon->SetMotionMagicAcceleration(acceleration_);
  talon->SetMotionMagicCruiseVelocity(cruise_velocity_);
}

void MotionMagicSettings::LogConfig(
    const std::shared_ptr<spdlog::logger> logger) const {
  assert(logger);
  logger->debug("control mode = motion magic");
  logger->debug("cruise velocity = {}", cruise_velocity_);
  logger->debug("acceleration = {}", acceleration_);
  PIDSettings::LogConfig(logger);
}
