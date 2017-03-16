#pragma once

#undef SPDLOG_TRACE_ON

#include <WPILib.h>
#include <cpptoml/cpptoml.h>
#include <spdlog/spdlog.h>

namespace steamworks {
namespace subsystem {

class Deadeye : public frc::Subsystem {
 public:
  Deadeye(const std::shared_ptr<cpptoml::table> config);
  virtual ~Deadeye();
  Deadeye& operator=(Deadeye&) = delete;
  Deadeye(Deadeye&) = delete;

  void Start();

  void SetGearLightEnabled(bool enable);
  void SetShooterLightEnabled(bool enable);

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
  // keep synced with deadeye
  enum MessageType {
    kNoTargetMesg = 0,
    kBoilerSolutionMesg = 1,
    kGearSolutionMesg = 2,
    kIdleMesg = 4,
  };

  const std::shared_ptr<spdlog::logger> logger_;
  int sockfd_;

  std::thread thread_;
  std::mutex mutex_;

  bool error_reported_ = false;

  // config settings
  int port_ = 5800;

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

  void Run();

  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
  void ConfigureNetworking();
};

} /* subsystem */
} /* steamworks */
