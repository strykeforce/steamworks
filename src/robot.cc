#include "robot.h"

#include <WPILibVersion.h>

#include "commands/commands.h"
#include "default_config.h"
#include "log.h"
#include "robot_map.h"
#include "sidewinder/version.h"
#include "version.h"

using namespace steamworks;
using namespace steamworks::command;

OI* Robot::oi = nullptr;
subsystem::Deadeye* Robot::deadeye = nullptr;
subsystem::Climber* Robot::climber = nullptr;
subsystem::SwerveDrive* Robot::drive = nullptr;
subsystem::GearLoader* Robot::gear_loader = nullptr;
subsystem::Hopper* Robot::hopper = nullptr;
subsystem::Intake* Robot::intake = nullptr;
subsystem::ShooterElevation* Robot::shooter_elevation = nullptr;
subsystem::ShooterWheel* Robot::shooter_wheel = nullptr;

Robot::Robot() : frc::IterativeRobot() {}

void Robot::RobotInit() {
  Configure();
  RobotMap::Initialize(config_);

  SPDLOG_TRACE(logger_, "initializing subsystems");
  logger_->info("running on {} robot",
                RobotMap::IsPracticeRobot() ? "PRACTICE" : "COMPETITION");
  deadeye = new subsystem::Deadeye(config_);
  deadeye->Start();
  climber = new subsystem::Climber(config_);
  drive = new subsystem::SwerveDrive(config_);
  gear_loader = new subsystem::GearLoader(config_);
  hopper = new subsystem::Hopper(config_);
  intake = new subsystem::Intake(config_);
  shooter_elevation = new subsystem::ShooterElevation(config_);
  shooter_wheel = new subsystem::ShooterWheel(config_);
  SPDLOG_TRACE(logger_, "done initializing subsystems");
  oi = new OI(config_);  // keep this after subsystems
}

void Robot::RobotPeriodic() {}

void Robot::DisabledInit() { SPDLOG_TRACE(logger_, "in DisabledInit"); }

void Robot::DisabledPeriodic() { frc::Scheduler::GetInstance()->Run(); }

void Robot::AutonomousInit() {
  SPDLOG_TRACE(logger_, "initializing autonomous mode");
  RobotMap::gyro->ZeroYaw();
  auto auton_mode = oi->GetAutonMode();
  logger_->info("initialize auton mode {:X}", auton_mode);
  switch (auton_mode) {
    case 1:
      autonomous_command_ = new auton::Sequence01();
      break;
    case 2:
      autonomous_command_ = new auton::Sequence02();
      break;
    case 3:
      autonomous_command_ = new auton::Sequence03();
      break;
    case 4:
      autonomous_command_ = new auton::Sequence04();
      break;
    case 5:
      autonomous_command_ = new auton::Sequence05();
      break;
    default:
      autonomous_command_ = new LogCommand("unrecognized command");
  }
  autonomous_command_->Start();
}

void Robot::AutonomousPeriodic() { frc::Scheduler::GetInstance()->Run(); }

void Robot::TeleopInit() {
  SPDLOG_TRACE(logger_, "checking auto gear load switch position");
  frc::Joystick fsj(OI::kFlightSimJoystick);
  if (fsj.GetRawButton(OI::kFlightSimLeftCornerDownButton)) {
    // button is already in auto on position so run command
    logger_->info("auto gear load is on");
  }

  if (fsj.GetRawButton(OI::kFlightSimLeftCornerUpButton)) {
    // button is already in auto on position so run command
    logger_->info("auto gear load is off");
  }
  stop_shooter_ = new StopShooting();
  stop_shooter_->Start();
}

void Robot::TeleopPeriodic() { frc::Scheduler::GetInstance()->Run(); }

void Robot::TestInit() { SPDLOG_TRACE(logger_, "in TestInit"); }

void Robot::TestPeriodic() { frc::LiveWindow::GetInstance()->Run(); }

/**
 Reads our configuration file from ~lvuser/steamworks.toml.
 * If not present or unable to parse, will read the default compiled-in
 * configuration from conf/default_config.toml.
 */
void Robot::Configure() {
  auto path = "/home/lvuser/steamworks.toml";
  // configure interim logger
  logger_ = spdlog::stdout_logger_st("config");
  LogVersion();
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
  Log::Initialize(config_);
  logger_ = spdlog::get("robot");
  spdlog::drop("config");
}

/**
 * Log the build version.
 */
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
