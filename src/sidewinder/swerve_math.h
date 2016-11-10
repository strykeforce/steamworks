#pragma once

#include <iostream>

namespace sidewinder {

struct DriveData {
  float fwd, str, rcw;
  float wsrf, wslf, wslr, wsrr;
  float warf, walf, walr, warr;
};

class SwerveMath {
 private:
  /* data */
 public:
  SwerveMath();
  void Calc(DriveData& dd);
};
} /* sidewinder */
