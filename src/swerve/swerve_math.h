#pragma once

#include <cpptoml/cpptoml.h>
#include <spdlog/fmt/ostr.h>

namespace sidewinder {
namespace swerve {
struct DriveData {
  double fwd, str, rcw;
  double wsrf, wslf, wslr, wsrr;
  double warf, walf, walr, warr;
};

struct Vector2f {
  double x;
  double y;
};

enum RotationPoint : unsigned char { Center, Shooter, END_OF_LIST };

enum WheelPoints : unsigned char {
  RightFront = 1,
  LeftFront = 0,
  LeftRear = 3,
  RightRear = 2,
};

class SwerveMath {
 private:
  double wheel_angles[4]{0.0, 0.0, 0.0, 0.0};
  double wheel_mags[4]{0.0, 0.0, 0.0, 0.0};
  double wheel_anglesPast[4]{0.0, 0.0, 0.0, 0.0};
  bool wheel_mag_negated[4]{false, false, false, false};

  struct Vector2f* rotation_vectors[RotationPoint::END_OF_LIST];

 public:
  SwerveMath(const std::shared_ptr<cpptoml::table> config);
  void operator()(DriveData& dd, enum RotationPoint rp);
};
}
} /* sidewinder */
