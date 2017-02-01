#include "link.h"

#include "cpptoml/cpptoml.h"
#include "spdlog/spdlog.h"

using namespace deadeye;

Link::Link(std::shared_ptr<cpptoml::table> config)
    : logger_(spdlog::get("deadeye")) {}

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
}
