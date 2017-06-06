#pragma once

#include <WPILib.h>

namespace steamworks {
namespace command {
namespace deadeye {

class EnableCamera : public frc::InstantCommand {
 public:
  enum class Mode { boiler, gear };
  EnableCamera(Mode);
  virtual ~EnableCamera() = default;

 protected:
  void Initialize() override;

 private:
  Mode mode_;
};

} /* deadeye */
} /* command */
} /* steamworks */
