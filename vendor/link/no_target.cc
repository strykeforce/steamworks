#include "no_target.h"

using namespace deadeye;

NoTargetSentence::NoTargetSentence() : Sentence() {
  name = "NTGT";
  parameters.push_back(std::to_string(kNoTarget));
}

NoTargetSentence::~NoTargetSentence() {}
