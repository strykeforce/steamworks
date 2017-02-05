#include "robot.h"

#include "WPILib.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "commands/log.h"
#include "default_config.h"
#include "robot_map.h"
#include "sidewinder/version.h"
#include "subsystems/drive.h"
#include "version.h"

using namespace steamworks;

OI* Robot::oi = nullptr;
subsystem::Drive* Robot::drive = nullptr;

Robot::Robot() : frc::IterativeRobot(), logger_(nullptr) { ConfigureLogging(); }

void Robot::RobotInit() {
  LogVersion();
  LoadConfig();
  RobotMap::Init(config_);

  oi = new OI(config_);
  drive = new subsystem::Drive(config_);
}

void Robot::DisabledInit() { logger_->trace(__FUNCTION__); }

void Robot::DisabledPeriodic() { frc::Scheduler::GetInstance()->Run(); }

void Robot::AutonomousInit() { logger_->trace(__FUNCTION__); }

void Robot::AutonomousPeriodic() { frc::Scheduler::GetInstance()->Run(); }

void Robot::TeleopInit() {
  logger_->trace(__FUNCTION__);
  frc::Joystick fsj(OI::kFlightSimJoystick);
  if (fsj.GetRawButton(OI::kFlightSimLeftCornerDownButton)) {
    // button is already in auto on position so run command
    logger_->info("auto gear load is on");
  }

  if (fsj.GetRawButton(OI::kFlightSimLeftCornerUpButton)) {
    // button is already in auto on position so run command
    logger_->info("auto gear load is off");
  }
}

void Robot::TeleopPeriodic() { frc::Scheduler::GetInstance()->Run(); }

void Robot::TestInit() { logger_->trace(__FUNCTION__); }

void Robot::TestPeriodic() { frc::LiveWindow::GetInstance()->Run(); }

/** Configure logging based on release type.
 * If building for Release, don't put ANSI terminal color codes in log since
 * it will be read in driver station logs.
 */
void Robot::ConfigureLogging() {
#ifdef NDEBUG
  logger_ = spdlog::stdout_logger_st("robot");
  logger_->set_level(spdlog::level::info);
  spdlog::stdout_logger_st("command")->set_level(spdlog::level::info);
  spdlog::stdout_logger_st("subsystem")->set_level(spdlog::level::info);
  logger_->info("configured as RELEASE build");
#else
  logger_ = spdlog::stdout_color_st("robot");
  logger_->set_level(spdlog::level::trace);
  spdlog::stdout_color_st("command")->set_level(spdlog::level::trace);
  spdlog::stdout_color_st("subsystem")->set_level(spdlog::level::trace);
  logger_->warn("configured as DEBUG build");
#endif
}

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
#ifdef NDEBUG
  logger_->info("configured as RELEASE build");
#else
  logger_->warn("configured as DEBUG build");
#endif
}

// used for: strings steamworks | grep STEAMWORKS_VERSION
static const char* steamworks_version =
    "STEAMWORKS_VERSION " STEAMWORKS_VERSION " " STEAMWORKS_COMPILE_DATE
    " " STEAMWORKS_COMPILE_TIME;

START_ROBOT_CLASS(steamworks::Robot)
