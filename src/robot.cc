#include "robot.h"

#include "WPILib.h"
#include "WPILibVersion.h"
#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "commands/commands.h"
#include "default_config.h"
#include "robot_map.h"
#include "sidewinder/version.h"
#include "subsystems/drive.h"
#include "version.h"

using namespace steamworks;

OI* Robot::oi = nullptr;
subsystem::Camera* Robot::camera = nullptr;
subsystem::Climber* Robot::climber = nullptr;
subsystem::SwerveDrive* Robot::drive = nullptr;
subsystem::GearLoader* Robot::gear_loader = nullptr;
subsystem::Hopper* Robot::hopper = nullptr;
subsystem::Intake* Robot::intake = nullptr;
subsystem::Shooter* Robot::shooter = nullptr;

Robot::Robot() : frc::IterativeRobot(), logger_(nullptr) { ConfigureLogging(); }

void Robot::RobotInit() {
  LogVersion();
  LoadConfig();
  RobotMap::Init(config_);

  logger_->trace("initializing subsystems");
  camera = new subsystem::Camera(config_);
  climber = new subsystem::Climber(config_);
  drive = new subsystem::SwerveDrive(config_);
  gear_loader = new subsystem::GearLoader(config_);
  hopper = new subsystem::Hopper(config_);
  intake = new subsystem::Intake(config_);
  shooter = new subsystem::Shooter(config_);
  logger_->trace("done initializing subsystems");
  oi = new OI(config_);  // keep this after subsystems
}

void Robot::RobotPeriodic() {}

void Robot::DisabledInit() { logger_->trace(__PRETTY_FUNCTION__); }

void Robot::DisabledPeriodic() { frc::Scheduler::GetInstance()->Run(); }

void Robot::AutonomousInit() { logger_->trace(__PRETTY_FUNCTION__); }

void Robot::AutonomousPeriodic() { frc::Scheduler::GetInstance()->Run(); }

void Robot::TeleopInit() {
  logger_->trace("checking auto gear load switch position");
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

void Robot::TestInit() { logger_->trace(__PRETTY_FUNCTION__); }

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
  spdlog::stdout_logger_st("sidewinder")->set_level(spdlog::level::info);
  logger_->info("configured as RELEASE build");
#else
  logger_ = spdlog::stdout_color_st("robot");
  logger_->set_level(spdlog::level::trace);
  spdlog::stdout_color_st("command")->set_level(spdlog::level::trace);
  spdlog::stdout_color_st("subsystem")->set_level(spdlog::level::trace);
  spdlog::stdout_color_st("sidewinder")->set_level(spdlog::level::trace);
  logger_->warn("configured as DEBUG build");
#endif
  spdlog::set_pattern("[%H:%M:%S.%e][%n][%l] %v");
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
  logger_->info("WPILib version: {}", WPILibVersion);
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
