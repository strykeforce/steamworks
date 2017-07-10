#pragma once

#include <cpptoml/cpptoml.h>
#include <spdlog/spdlog.h>

namespace deadeye {
class Link {
 public:
  enum Mode {
    idle = 0,
    boiler,
    gear,
  };

  Link(std::shared_ptr<cpptoml::table> config);
  Link(const Link&) = delete;
  Link& operator=(const Link&) = delete;

  void Start();

  int GetMode();
  void SendBoilerSolution(int azimuth_error, int centerline_error);
  void SendGearSolution(int azimuth_error, int range);
  void SendNoTarget();

 private:
  enum MessageType {
    kNoTargetMesg = 0,
    kBoilerSolutionMesg = 1,
    kGearSolutionMesg = 2,
    kIdleMesg = 4,
  };

  std::shared_ptr<spdlog::logger> logger_;
  std::thread thread_;
  std::mutex mutex_;
  std::string address_ = "10.27.67.2";
  int port_;
  int sendfd_;
  int recvfd_;
  int current_mode_ = Mode::idle;

  void LoadConfigSettings(const std::shared_ptr<cpptoml::table> config);
  void ConfigureNetworking();

  void Run();
};

} /* deadeye */
