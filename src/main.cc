#include <cstdlib>

#include "spdlog/spdlog.h"
#include "cpptoml/cpptoml.h"

// #include "boiler_camera.h"
#include "deadeye.h"

int main(int argc, char const* argv[]) {
  auto logger = spdlog::stdout_color_st("deadeye");
  logger->set_level(spdlog::level::debug);

  // read config file from path specified in DEADEYE_CONF env variable or
  // default to /etc/deadeye.toml
  const char* conf_path = std::getenv("DEADEYE_CONF");
  if (!conf_path) {
    conf_path = "/etc/deadeye/deadeye.toml";
  }
  logger->info("reading configuration from {}", conf_path);
  auto config = cpptoml::parse_file(conf_path);

  // configure camera
  // auto camera = GetCamera(config);

  // start processing
  deadeye::Deadeye deadeye(config);
  deadeye.Run();
  return 0;
}
