#pragma once

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

  // Mode GetMode();
  // int GetAzimuthError();
  // int GetRange();

 private:
  const std::shared_ptr<spdlog::logger> logger_;
  std::string port_{"/dev/ttyUSB0"};
  uint32_t speed_ = 115200;
  // Mode mode_;
  // int azimuth_error_;
  // int range_;
  std::thread thread_;
  std::atomic_bool stop_thread_;
  std::mutex mutex_;

  void Run();
  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
};

} /* subsystem */
} /* steamworks */
