#include <cstdlib>
#include "spdlog/spdlog.h"
#include "cpptoml/cpptoml.h"

#include "camera.h"
#include "deadeye.h"

using namespace deadeye;

int main(int argc, char const* argv[]) {
  auto logger = spdlog::stdout_color_st("deadeye");
  const char* conf_path = std::getenv("DEADEYE_CONF");
  if (!conf_path) {
    conf_path = "/etc/deadeye.toml";
  }
  logger->info("reading configuration from {}", conf_path);

  auto config = cpptoml::parse_file(conf_path);
  auto camera = GetCamera(config);

  Deadeye deadeye(camera);
  deadeye.Display();
  return 0;
}
