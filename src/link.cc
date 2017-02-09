#include "link.h"

#include <string>

#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

#include "link/boiler.h"

using namespace deadeye;
using namespace std;

Link::Link(std::shared_ptr<cpptoml::table> config)
    : logger_(spdlog::get("deadeye")), serial_(nullptr) {
  assert(config);

  string port = "/dev/null";
  int speed = 115200;
  int timeout = 1000;

  auto link_config = config->get_table("LINK");
  if (link_config) {
    auto port_setting = link_config->get_as<string>("port");
    if (port_setting) {
      port = *port_setting;
    } else {
      logger_->warn("LINK port setting missing, using default");
    }

    auto speed_setting = link_config->get_as<int>("speed");
    if (speed_setting) {
      speed = *speed_setting;
    } else {
      logger_->warn("LINK port speed setting missing, using default");
    }

    auto timeout_setting = link_config->get_as<int>("timeout");
    if (timeout_setting) {
      timeout = *timeout_setting;
    } else {
      logger_->warn("LINK timeout setting missing, using default");
    }
  } else {
    logger_->error("LINK configuration section missing, using defaults");
  }
  logger_->info("link on port {} at {} baud with {}ms timeout", port, speed,
                timeout);

  serial_.reset(
      new serial::Serial(port, speed, serial::Timeout::simpleTimeout(timeout)));
}

/** Read serial line for current mode.
 */
Link::Mode Link::GetMode() {
#ifdef DEADEYE_TEST
  return kQuitMode;
#else
  return kBoilerMode;
#endif
}

/** Send the boiler shooting solution across the serial line.
 */
void Link::SendBoilerSolution(int azimuth_error,
                              int range,
                              int angle,
                              int speed) {
  logger_->debug(
      "boiler solution: az err = {}, range = {}, angle = {}, speed = {}",
      azimuth_error, range, angle, speed);
  BoilerSentence bts(azimuth_error, range, angle, speed);
  serial_->write(bts.ToString() + "\n");
}
