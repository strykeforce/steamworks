#pragma once

namespace sidewinder {

struct DriveData {
  float fwd, str, rcw;
  float ws1, ws2, ws3, ws4;
  float wa1, wa2, wa3, wa4;
};

class SwerveMath {
 private:
  /* data */
 public:
  SwerveMath();
  void Calc(DriveData& dd);
};
} /* sidewinder */
