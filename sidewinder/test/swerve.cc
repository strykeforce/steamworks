#include <algorithm>
#include <cmath>
#include <vector>

#include "catch/catch.hpp"

#include "swerve/swerve_math.h"
#include "swerve_test_data.h"

using namespace sidewinder::swerve;

SwerveMath math = SwerveMath();

TEST_CASE("convert joystick input to wheel output", "[swerve]") {
  DriveData tc;
  for (const auto& ex : tests) {
    tc.fwd = ex.fwd;
    tc.str = ex.str;
    tc.rcw = ex.rcw;
    math(tc);
    REQUIRE(tc.wsrf == Approx(ex.wsrf));
    REQUIRE(tc.wslf == Approx(ex.wslf));
    REQUIRE(tc.wslr == Approx(ex.wslr));
    REQUIRE(tc.wsrr == Approx(ex.wsrr));
    REQUIRE(tc.warf == Approx(ex.warf).epsilon(0.01));
    REQUIRE(tc.walf == Approx(ex.walf).epsilon(0.01));
    REQUIRE(tc.walr == Approx(ex.walr).epsilon(0.01));
    REQUIRE(tc.warr == Approx(ex.warr).epsilon(0.01));
  }
}
