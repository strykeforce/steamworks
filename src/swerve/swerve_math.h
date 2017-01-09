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
  DriveData last_dd_;

 public:
  SwerveMath();
  void operator()(DriveData& dd);
};
} /* swerve */
} /* sidewinder */
