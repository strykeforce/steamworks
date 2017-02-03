#include <vector>

#include "catch/catch.hpp"

#include "link/sentence.h"

using namespace sfrt;

TEST_CASE("valid checksum", "[sentence]") {
  Sentence sentence;

  sentence.parsed_checksum = 11;
  sentence.calculated_checksum = 11;
  REQUIRE(sentence.ChecksumOK());
}

TEST_CASE("zero checksum", "[sentence]") {
  Sentence sentence;

  REQUIRE_FALSE(sentence.ChecksumOK());

  sentence.parsed_checksum = 1;
  REQUIRE_FALSE(sentence.ChecksumOK());

  sentence.parsed_checksum = 0;
  sentence.calculated_checksum = 0;
  REQUIRE_FALSE(sentence.ChecksumOK());
}

TEST_CASE("invalid checksum", "[sentence]") {
  Sentence sentence;

  sentence.parsed_checksum = 1;
  sentence.calculated_checksum = 2;
  REQUIRE_FALSE(sentence.ChecksumOK());
}

TEST_CASE("sentence initially invalid", "[sentence]") {
  Sentence sentence;

  REQUIRE_FALSE(sentence.Valid());
}
