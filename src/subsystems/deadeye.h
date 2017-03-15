#pragma once

#undef SPDLOG_TRACE_ON

#include <WPILib.h>
#include <cpptoml/cpptoml.h>
#include <serial/serial.h>
#include <spdlog/spdlog.h>

#include "mode.h"
#include "parser.h"
#include "sentence.h"

namespace steamworks {
namespace subsystem {

class Deadeye : public frc::Subsystem {
 public:
  Deadeye(const std::shared_ptr<cpptoml::table> config);
  virtual ~Deadeye();
  Deadeye& operator=(Deadeye&) = delete;
  Deadeye(Deadeye&) = delete;

  void Communicate();

  void SetGearLightEnabled(bool enable);
  void SetShooterLightEnabled(bool enable);

  deadeye::Mode GetMode();
  void SetMode(deadeye::Mode mode);

  int GetAzimuthError();
  int GetCenterlineError();

  bool HasTarget();

  // SHOOTER SOLUTION CALCULATION
  // void CalculateSolution(int centerline_elevation, int
  // centerline_pixel_error);
  bool CalculateSolution(int centerline_elevation);
  double GetSolutionRange();
  double GetSolutionElevation();
  double GetSolutionWheelSpeed();
  double GetSolutionAzimuthOffset();

 private:
  // can replace the init_* states with on_enter transitions
  enum class State {
    init,
    init_mode,
    boiler,
    gear,
    idle,
    error,
  };

  const std::shared_ptr<spdlog::logger> logger_;
  std::unique_ptr<frc::Notifier> notifier_;
  std::mutex mutex_;
  bool error_reported_ = false;

  // config settings
  std::string port_{"/dev/ttyUSB0"};
  uint32_t speed_ = 115200;
  int period_ = 20;

  // communication
  deadeye::Mode mode_ = deadeye::Mode::idle;
  deadeye::Mode new_mode_ = deadeye::Mode::idle;
  State state_{State::init};
  std::unique_ptr<serial::Serial> serial_;
  deadeye::Parser parser_;
  deadeye::Sentence sentence_;

  // current deadeye parameters
  bool has_target_ = false;
  int azimuth_error_;
  int centerline_error_;
  int range_;

  // shooter solution outputs
  double solution_elevation_;
  double solution_wheel_speed_;
  double solution_range_;
  double solution_azimuth_offset_;

  // shooter solution config constants from config file
  double elevation_zero_angle_ = 0.0;  // angle corresponding to encoder zero
  double centerline_height_ = 0.0;  // height of target centerline above ground
  double camera_height_ = 0.0;      // camera focal point distance from ground
  double camera_angle_ = 0.0;       // camera to shooter offset angle
  double degrees_per_tick_ = 0.00001;

  State CheckMode();
  State ReadSentence();
  void SendSentence(const deadeye::Sentence& sentence);

  // states
  State DoInit();
  State DoIdle();
  State DoInitMode();
  State DoBoiler();
  State DoGear();
  State DoError();

  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
};

} /* subsystem */
} /* steamworks */
