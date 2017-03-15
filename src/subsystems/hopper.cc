#include "hopper.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "sidewinder/talon/settings.h"

#include "robot_map.h"

using namespace steamworks::subsystem;
using namespace sidewinder;

Hopper::Hopper(const std::shared_ptr<cpptoml::table> config)
    : frc::Subsystem("Hopper"),
      logger_(spdlog::get("subsystem")),
      voltage_(7.0),
      is_running_(false) {
  LoadConfigSettings(config);
}

void Hopper::Start() {
  RobotMap::hopper_talon->Set(voltage_);
  is_running_ = true;
#if !NDEBUG
  SmartDashboard::PutNumber("Hopper Voltage", voltage_);
#endif
}

void Hopper::Stop() {
  RobotMap::hopper_talon->StopMotor();
  is_running_ = false;
}

bool Hopper::IsRunning() { return is_running_; }

double Hopper::GetVoltage() { return voltage_; }

void Hopper::SetVoltage(double volts) {
  voltage_ = volts;
  Start();
}

void Hopper::LoadConfigSettings(
    const std::shared_ptr<cpptoml::table> config_in) {
  auto config = config_in->get_table("STEAMWORKS")->get_table("HOPPER");
  if (!config) {
    throw std::invalid_argument("STEAMWORKS.HOPPER table missing from config");
  }

  auto d_opt = config->get_as<double>("voltage");
  if (d_opt) {
    voltage_ = *d_opt;
  } else {
    logger_->warn("STEAMWORKS.HOPPER voltage setting missing, using default");
  }
  logger_->info("hopper motor voltage: {}", voltage_);

  auto talon_settings = talon::Settings::Create(config, "hopper");
  talon_settings->Initialize(RobotMap::hopper_talon);
  logger_->info("hopper Talon initialized");
}