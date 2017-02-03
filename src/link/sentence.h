#pragma once

#include <string>
#include <vector>
#include <cstdint>

// #include "parser.h"

namespace sfrt {

class Sentence {
  friend class Parser;

 private:
  bool is_valid_;

 public:
  std::string text;
  std::string name;
  std::vector<std::string> parameters;
  std::string checksum;
  uint8_t parsed_checksum;
  uint8_t calculated_checksum;

  Sentence();
  virtual ~Sentence();

  bool ChecksumOK() const;
  bool Valid() const;
};

} /* sfrt */
