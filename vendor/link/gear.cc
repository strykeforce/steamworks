#include "gear.h"

using namespace deadeye;
using namespace std;

GearSentence::GearSentence() : Sentence() {}

GearSentence::GearSentence(int azimuth_error, int range)
    : Sentence(), azimuth_error(azimuth_error), range(range) {
  name = "GAZ";
  parameters.push_back(std::to_string(kGear));
  parameters.push_back(to_string(azimuth_error));
  parameters.push_back(to_string(range));
}

GearSentence::~GearSentence() {}
