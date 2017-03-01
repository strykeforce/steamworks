#pragma once

#include "sentence.h"

namespace deadeye {
class BoilerSentence : public Sentence {
 public:
  int azimuth_error;
  int range;
  int angle;
  int speed;

  BoilerSentence();
  BoilerSentence(int azimuth_error, int range, int angle, int speed);
  virtual ~BoilerSentence();
};

} /* deadeye */
