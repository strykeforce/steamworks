#pragma once

#include "config.h"

#ifdef LOG_FPS
#include <fstream>
#endif

#include <WPILib.h>
#include <cpptoml/cpptoml.h>
#include <netinet/in.h>
#include <spdlog/spdlog.h>
#include <sys/socket.h>

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

  void EnableBoilerCamera();
  void EnableGearCamera();

  int GetAzimuthError();
  int GetStrafeError();
  int GetCenterlineError();
  int GetTargetHeight();

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
    kErrorMesg = 5,
  };

  const std::shared_ptr<spdlog::logger> logger_;
  int recvfd_;
  int sendfd_;
  std::string remote_;
  struct sockaddr_in remote_addr_;

  std::thread thread_;
  std::mutex mutex_;

  // config settings
  int port_ = 5800;

  // current deadeye parameters
  bool has_target_ = false;
  int azimuth_error_;
  int centerline_error_;
  int target_height_;

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

#ifdef LOG_FPS
  frc::Timer timer_;
  double telemetry_start_;
  double last_frame_;
  std::unique_ptr<std::ofstream> telemetry_;
  void InitializeTelemetry();
  void LogTelemetry();
  void EndTelemetry();
#endif
};

} /* subsystem */
} /* steamworks */
