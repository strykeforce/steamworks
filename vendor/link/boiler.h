#pragma once

#include "sentence.h"

namespace deadeye {
class BoilerSentence : public Sentence {
 public:
  int azimuth_error;
  int centerline_error;

  BoilerSentence();
  BoilerSentence(int azimuth_error, int centerline_error);
  virtual ~BoilerSentence();
};

} /* deadeye */
