#include "catch/catch.hpp"
#include "cpptoml/cpptoml.h"

#include "deadeye.h"

using namespace deadeye;

TEST_CASE("deadeye Run method", "[deadeye]") {
  auto root = cpptoml::make_table();
  auto camera = cpptoml::make_table();
  root->insert("CAMERA", camera);
  camera->insert("width", 1280);
  camera->insert("height", 1080);
  camera->insert("exposure", -1.0);
  Deadeye deadeye{root};
  REQUIRE_NOTHROW(deadeye.Run());
}
