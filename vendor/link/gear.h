#pragma once

#include "sentence.h"

namespace deadeye {
class GearSentence : public Sentence {
 public:
  int azimuth_error;
  int range;

  GearSentence();
  GearSentence(int azimuth_error, int range);
  virtual ~GearSentence();
};

} /* deadeye */
