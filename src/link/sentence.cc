#include "sentence.h"

#include <string>
#include <vector>
#include <cstdint>

using namespace sfrt;

Sentence::Sentence()
    : is_valid_(false), parsed_checksum(0), calculated_checksum(0) {}

Sentence::~Sentence() {}

bool Sentence::ChecksumOK() const {
  return (parsed_checksum != 0 && calculated_checksum != 0) &&
         (parsed_checksum == calculated_checksum);
}

bool Sentence::Valid() const {
  return is_valid_ && ChecksumOK();
}
