#include "mode.h"

using namespace deadeye;

ModeSentence::ModeSentence() : Sentence() {}

ModeSentence::ModeSentence(Mode mode) : Sentence(), mode(mode) {
  name = "MODE";
  parameters.push_back("3");
  parameters.push_back(std::to_string(static_cast<int>(mode)));
}

ModeSentence::~ModeSentence() {}
