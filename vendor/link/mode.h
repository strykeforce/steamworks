#pragma once

#include "sentence.h"

namespace deadeye {

enum class Mode { boiler, gear, idle };

class ModeSentence : public Sentence {
 public:
  Mode mode;

  ModeSentence();
  ModeSentence(Mode mode);
  virtual ~ModeSentence();
};

} /* deadeye */
