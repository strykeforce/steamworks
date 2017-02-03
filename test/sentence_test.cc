#include <vector>

#include "catch/catch.hpp"

#include "link/sentence.h"

using namespace deadeye;

TEST_CASE("valid checksum", "[link][sentence]") {
  Sentence sentence;

  sentence.parsed_checksum = 11;
  sentence.calculated_checksum = 11;
  REQUIRE(sentence.ChecksumOK());
}

TEST_CASE("zero checksum", "[link][sentence]") {
  Sentence sentence;

  REQUIRE_FALSE(sentence.ChecksumOK());

  sentence.parsed_checksum = 1;
  REQUIRE_FALSE(sentence.ChecksumOK());

  sentence.parsed_checksum = 0;
  sentence.calculated_checksum = 0;
  REQUIRE_FALSE(sentence.ChecksumOK());
}

TEST_CASE("invalid checksum", "[link][sentence]") {
  Sentence sentence;

  sentence.parsed_checksum = 1;
  sentence.calculated_checksum = 2;
  REQUIRE_FALSE(sentence.ChecksumOK());
}

TEST_CASE("sentence initially invalid", "[link][sentence]") {
  Sentence sentence;

  REQUIRE_FALSE(sentence.Valid());
}

TEST_CASE("sentence to string", "[link][sentence]") {
  Sentence sentence;
  sentence.name = "FOO";
  REQUIRE(sentence.ToString() == "$FOO*46");

  sentence.parameters.push_back("10");
  REQUIRE(sentence.ToString() == "$FOO,10*6B");

  sentence.parameters.push_back("20");
  sentence.parameters.push_back("abc");
  REQUIRE(sentence.ToString() == "$FOO,10,20,abc*09");
}
