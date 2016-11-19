#include "cannon.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

using namespace avenger;

Cannon::Cannon(const std::shared_ptr<cpptoml::table> config)
    : ::Subsystem("Cannon"), logger_(spdlog::stdout_color_st(GetName())) {
  logger_->set_level(spdlog::level::trace);
  fire_duration_sec_ =
      config->get_as<double>("fire_duration_ms").value_or(0) / 1000.0;

  logger_->info("firing pulse is {} ms", fire_duration_sec_ * 1000);

  for (size_t i = 0; i < 8; i++) {
    solenoids_.push_back(std::unique_ptr<::Solenoid>(new ::Solenoid(i)));
  }

  notifier_ = std::make_unique<::Notifier>(&Cannon::CloseSolenoid, this);
}

void Cannon::Fire() {
  std::lock_guard<std::mutex> lock(mutex_);

  if (firing_) return;
  if (barrel_ == 7) barrel_ = -1;

  solenoids_[++barrel_]->Set(true);
  firing_ = true;
  notifier_->StartSingle(fire_duration_sec_);
  logger_->info("fired barrel {}", barrel_);
}

void Cannon::CloseSolenoid() {
  std::lock_guard<std::mutex> lock(mutex_);

  solenoids_[barrel_]->Set(false);
  firing_ = false;
  logger_->debug("closed solenoid for barrel {}", barrel_);
}
