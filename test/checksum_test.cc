#include <cstdio>

#include "catch/catch.hpp"

#include "link/checksum.h"

using namespace deadeye;

TEST_CASE("to hex string", "[link][checksum]") {
  std::string a = ToHex(45);
  REQUIRE(a == "2D");
}
