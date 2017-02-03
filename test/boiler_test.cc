#include <vector>

#include "catch/catch.hpp"

#include "link/boiler.h"
#include "link/parser.h"

using namespace deadeye;

TEST_CASE("boiler constructor", "[link][boiler][sentence]") {
  BoilerSentence boiler(1, 2, 3, 4);
  REQUIRE(boiler.azimuth_error == 1);
  REQUIRE(boiler.range == 2);
  REQUIRE(boiler.angle == 3);
  REQUIRE(boiler.speed == 4);
  REQUIRE_FALSE(boiler.Valid());
  boiler.parsed_checksum = boiler.calculated_checksum = 1;
  REQUIRE(boiler.ChecksumOK());
}

TEST_CASE("boiler valid checksum", "[link][boiler][sentence]") {
  BoilerSentence sentence;

  sentence.parsed_checksum = 11;
  sentence.calculated_checksum = 11;
  REQUIRE(sentence.ChecksumOK());
}

TEST_CASE("boiler parses", "[link][boiler][parser]") {
  Parser parser;
  BoilerSentence sentence;
  parser.ParseText(sentence, "$BTS,-10,100,15,20*5E");
  REQUIRE(sentence.parameters.size() == 4);
  REQUIRE(sentence.name == "BTS");
  REQUIRE(sentence.Valid());
  REQUIRE(sentence.parameters[0] == "-10");
  REQUIRE(sentence.parameters[1] == "100");
  REQUIRE(sentence.parameters[2] == "15");
  REQUIRE(sentence.parameters[3] == "20");
}

TEST_CASE("boiler to string", "[link][boiler]") {
  BoilerSentence bts(111, 222, 333, 444);
  REQUIRE(bts.ToString() == "$BTS,111,222,333,444*41");
}
