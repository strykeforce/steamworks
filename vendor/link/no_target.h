#pragma once

#include "sentence.h"

namespace deadeye {
class NoTargetSentence : public Sentence {
 public:
  NoTargetSentence();
  virtual ~NoTargetSentence();
};

} /* deadeye */
