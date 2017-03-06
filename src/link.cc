#include "link.h"

#include <string>

#include "link/boiler.h"
#include "link/gear.h"
#include "link/no_target.h"
#include "link/mode.h"

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

/**
 * Read serial line for current mode.
 */
Mode Link::GetMode() {
#ifdef DEADEYE_TEST
  return Mode::quit;
#else
  return Mode::boiler;
#endif
}

/**
 *Send the boiler shooting solution across the serial line.
 */
void Link::SendBoilerSolution(int azimuth_error, int centerline_error) {
  logger_->debug("boiler solution: az err = {}, centerline_error = {}", azimuth_error,
                 centerline_error);
  BoilerSentence bts(azimuth_error, centerline_error);
  serial_->write(bts.ToString() + "\n");
}

/**
 * Send the gear azimuth error.
 */
void Link::SendGearSolution(int azimuth_error, int range) {
  logger_->debug("gear solution: az error = {}, range = {}", azimuth_error,
                 range);
  GearSentence gaz(azimuth_error, range);
  serial_->write(gaz.ToString() + "\n");
}

/**
 * Send a no target message on the serial line.
 */
void Link::SendNoTarget() {
  logger_->debug("no target");
  NoTargetSentence nts;
  serial_->write(nts.ToString() + "\n");
}
