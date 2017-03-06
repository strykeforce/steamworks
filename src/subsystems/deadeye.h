#pragma once

#include <WPILib.h>
#include <cpptoml/cpptoml.h>
#include <spdlog/spdlog.h>

#include "mode.h"

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

  deadeye::Mode GetMode();
  // void SetMode(deadeye::Mode mode);

  int GetAzimuthError();
  int GetCenterlineError();

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  std::string port_{"/dev/ttyUSB0"};
  uint32_t speed_ = 115200;
  deadeye::Mode mode_ = deadeye::Mode::quit;
  int azimuth_error_;
  int centerline_error_;
  int range_;
  std::thread thread_;
  std::atomic_bool stop_thread_;
  std::mutex mutex_;
  bool error_reported_ = false;

  void Run();
  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
};

} /* subsystem */
} /* steamworks */
