#include "catch/catch.hpp"
#include "cpptoml/cpptoml.h"

#include <cstdlib>
#include <fcntl.h>
#include <string>

#include "deadeye.h"

using namespace deadeye;

TEST_CASE("deadeye Run method", "[deadeye]") {
  auto root = cpptoml::make_table();
  auto camera = cpptoml::make_table();
  root->insert("CAMERA", camera);
  camera->insert("width", 1280);
  camera->insert("height", 1080);
  camera->insert("exposure", -1.0);

  // create a fake serial port out of a pseudo-tty
  int pts = posix_openpt(O_RDWR | O_NOCTTY);
  char* port = ttyname(pts);
  REQUIRE(port != NULL);

  auto link = cpptoml::make_table();
  root->insert("LINK", link);
  link->insert("port", std::string(port));

  Deadeye deadeye{root};
  REQUIRE_NOTHROW(deadeye.Run());
}
