#include "boiler.h"

using namespace deadeye;

BoilerSentence::BoilerSentence() : Sentence() {}

BoilerSentence::BoilerSentence(int azimuth_error, int centerline_error)
    : Sentence(),
      azimuth_error(azimuth_error),
      centerline_error(centerline_error) {
  name = "BTS";
  parameters.push_back(std::to_string(kBoiler));
  parameters.push_back(std::to_string(azimuth_error));
  parameters.push_back(std::to_string(centerline_error));
}

BoilerSentence::~BoilerSentence() {}
