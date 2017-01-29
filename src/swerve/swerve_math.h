#pragma once

#include <iostream>

#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

namespace sidewinder {
namespace swerve {
struct DriveData {
  double fwd, str, rcw;
  double wsrf, wslf, wslr, wsrr;
  double warf, walf, walr, warr;
};

class SwerveMath {
 private:
  double wheel_angles[4]{0.0, 0.0, 0.0, 0.0};
  double wheel_mags[4]{0.0, 0.0, 0.0, 0.0};
  double wheel_anglesPast[4]{0.0, 0.0, 0.0, 0.0};
  bool wheel_mag_negated[4]{false, false, false, false};

 public:
  SwerveMath();
  void operator()(DriveData& dd);
};
}
} /* sidewinder */
