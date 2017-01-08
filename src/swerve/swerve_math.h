#pragma once

#include <iostream>

#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

namespace sidewinder {

struct DriveData {
  float fwd, str, rcw;
  float wsrf, wslf, wslr, wsrr;
  float warf, walf, walr, warr;
};

class SwerveMath {
 private:
  DriveData last_dd_;

 public:
  SwerveMath();
  void operator()(DriveData& dd);
};
} /* sidewinder */
