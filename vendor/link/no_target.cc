#include "no_target.h"

using namespace deadeye;

NoTargetSentence::NoTargetSentence() : Sentence() {
  name = "NTGT";
  parameters.push_back("0");
}

NoTargetSentence::~NoTargetSentence() {}
