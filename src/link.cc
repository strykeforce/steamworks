#include "link.h"

#include <string>

#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

using namespace deadeye;
using namespace std;

Link::Link(std::shared_ptr<cpptoml::table> config)
    : logger_(spdlog::get("deadeye")), serial_(nullptr) {
  serial_.reset(new serial::Serial("/dev/ttyTHS2", 115200,
                                   serial::Timeout::simpleTimeout(1000)));
}

/** Read serial line for current mode.
 */
Link::Mode Link::GetMode() {
  return kBoilerMode;
}

/** Send the boiler shooting solution across the serial line.
 */
void Link::SendBoilerSolution(int azimuth_error,
                              int range,
                              int angle,
                              int speed) {
  logger_->info(
      "boiler solution: az err = {}, range = {}, angle = {}, speed = {}",
      azimuth_error, range, angle, speed);
  serial_->write("$BTS," + to_string(azimuth_error) + "," + to_string(range) +
                 "," + to_string(angle) + "," + to_string(speed) + "*4E\r\n");
}
