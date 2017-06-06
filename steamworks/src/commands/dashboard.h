#pragma once

#include <WPILib.h>

namespace steamworks {
namespace command {

class DashboardLight : public frc::InstantCommand {
 public:
  DashboardLight(std::string label, bool state);

 protected:
  void Initialize() override;

 private:
  std::string label_;
  bool state_;
};
} /* command */
} /* steamworks */
