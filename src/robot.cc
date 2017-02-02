#include "robot.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "default_config.h"
#include "robot_map.h"
#include "sidewinder/version.h"
#include "subsystems/drive.h"
#include "version.h"

using namespace steamworks;

OI* Robot::oi = nullptr;
Drive* Robot::drive = nullptr;

Robot::Robot() : IterativeRobot(), logger_(spdlog::stdout_color_st("robot")) {
  logger_->set_level(spdlog::level::trace);
}

void Robot::RobotInit() {
  LogVersion();
  LoadConfig();
  RobotMap::Init(config_);

  oi = new OI(config_);
  drive = new Drive(config_);
}

void Robot::DisabledInit() { logger_->trace("DisabledInit"); }

void Robot::DisabledPeriodic() { ::Scheduler::GetInstance()->Run(); }

void Robot::AutonomousInit() { logger_->trace("AutonomousInit"); }

void Robot::AutonomousPeriodic() { ::Scheduler::GetInstance()->Run(); }

void Robot::TeleopInit() { logger_->trace("TeleopInit"); }

void Robot::TeleopPeriodic() { ::Scheduler::GetInstance()->Run(); }

void Robot::TestInit() { logger_->trace("TestInit"); }

void Robot::TestPeriodic() { ::LiveWindow::GetInstance()->Run(); }

/** Reads our configuration file from ~lvuser/steamworks.toml.
 * If not present or unable to parse, will read the default compiled-in
 * configuration from conf/default_config.toml.
 */
void Robot::LoadConfig() {
  auto path = "/home/lvuser/steamworks.toml";
  logger_->info("loading configuration from: {}", path);

  try {
    config_ = cpptoml::parse_file(path);
    return;
  } catch (const std::exception& e) {
    logger_->warn(e.what());
  }

  logger_->warn("using default config compiled into binary");
  std::string conf(default_config_toml, std::end(default_config_toml));
  std::istringstream is{conf};
  cpptoml::parser p{is};
  config_ = p.parse();
}

void Robot::LogVersion() {
  logger_->info("STEAMWORKS {}.{}.{}{} initializing...",
                STEAMWORKS_VERSION_MAJOR, STEAMWORKS_VERSION_MINOR,
                STEAMWORKS_VERSION_PATCH, STEAMWORKS_VERSION_META);
  logger_->info(" build {} compiled at {} {}", STEAMWORKS_VERSION_BUILD,
                STEAMWORKS_COMPILE_DATE, STEAMWORKS_COMPILE_TIME);
  logger_->info(sidewinder::GetVersion());
}

// used for: strings steamworks | grep STEAMWORKS_VERSION
static const char* steamworks_version =
    "STEAMWORKS_VERSION " STEAMWORKS_VERSION " " STEAMWORKS_COMPILE_DATE
    " " STEAMWORKS_COMPILE_TIME;

START_ROBOT_CLASS(steamworks::Robot)
