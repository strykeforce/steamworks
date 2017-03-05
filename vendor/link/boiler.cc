#include "boiler.h"

using namespace deadeye;

BoilerSentence::BoilerSentence() : Sentence() {}

BoilerSentence::BoilerSentence(int azimuth_error,
                               int range,
                               int angle,
                               int speed)
    : Sentence(),
      azimuth_error(azimuth_error),
      range(range),
      angle(angle),
      speed(speed) {
  name = "BTS";
  parameters.push_back("1");
  parameters.push_back(std::to_string(azimuth_error));
  parameters.push_back(std::to_string(range));
  parameters.push_back(std::to_string(angle));
  parameters.push_back(std::to_string(speed));
}

BoilerSentence::~BoilerSentence() {}
