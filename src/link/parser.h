#pragma once

#include <string>
#include <cstdint>

#include "sentence.h"

namespace sfrt {

class Parser {
 private:
  uint8_t FromHex(const char a);

 public:
  Parser();
  virtual ~Parser();

  /** Fills the sfrt object with the results of parsing the string.
   */
  void ParseText(Sentence& sentence, std::string txt);
  uint8_t CalculateChecksum(std::string s);
};

} /* sfrt */
