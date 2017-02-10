#include "catch/catch.hpp"

#include "link/parser.h"

using namespace deadeye;

TEST_CASE("zero length line invalid", "[link][parser]") {
  Parser parser;
  Sentence sentence;
  parser.ParseText(sentence, "");
  REQUIRE_FALSE(sentence.Valid());
}

TEST_CASE("received line saved", "[link][parser]") {
  Parser parser;
  Sentence sentence;
  parser.ParseText(sentence, "abc");
  REQUIRE(sentence.text == "abc");
}

TEST_CASE("line without $ invalid", "[link][parser]") {
  Parser parser;
  Sentence sentence;
  parser.ParseText(sentence, "abc");
  REQUIRE_FALSE(sentence.Valid());
}

TEST_CASE("line without * invalid", "[link][parser]") {
  Parser parser;
  Sentence sentence;
  parser.ParseText(sentence, "$def,1");
  REQUIRE_FALSE(sentence.Valid());
}

TEST_CASE("checksum calculation", "[link][parser]") {
  Parser parser;
  Sentence sentence;
  parser.ParseText(sentence, "$ABCDEF*07");
  REQUIRE(sentence.calculated_checksum == 7);

  parser.ParseText(sentence, "$LMNOP,1,2,3*4C");
  REQUIRE(sentence.calculated_checksum == 76);

  parser.ParseText(sentence, "$A,100,200*42");
  REQUIRE(sentence.calculated_checksum == 66);
}

TEST_CASE("$, invalid", "[link][parser]") {
  Parser parser;
  Sentence sentence;
  parser.ParseText(sentence, "$,1,2,3*1C");
  REQUIRE_FALSE(sentence.Valid());
}

TEST_CASE("sets name", "[link][parser]") {
  Parser parser;
  Sentence sentence;
  parser.ParseText(sentence, "$A,100,200*42");
  REQUIRE(sentence.name == "A");
}

TEST_CASE("sets empty parameters", "[link][parser]") {
  Parser parser;
  Sentence sentence;
  parser.ParseText(sentence, "$ABC,*6C");
  for (const auto& p : sentence.parameters)
    WARN("parameter = " << p);
  REQUIRE(sentence.parameters.size() == 0);
}

TEST_CASE("sets some parameters", "[link][parser]") {
  Parser parser;
  Sentence sentence;
  parser.ParseText(sentence, "$ABC,2,12.9,1000*4B");
  REQUIRE(sentence.parameters.size() == 3);
  REQUIRE(sentence.name == "ABC");
  REQUIRE(sentence.Valid());
  REQUIRE(sentence.parameters[0] == "2");
  REQUIRE(sentence.parameters[1] == "12.9");
  REQUIRE(sentence.parameters[2] == "1000");
}

TEST_CASE("gets two sentences", "[link][parser]") {
  Parser parser;
  Sentence sentence;
  parser.ParseText(sentence, "$ABC,2,12.9,1000*4B");
  REQUIRE(sentence.parameters.size() == 3);
  REQUIRE(sentence.name == "ABC");
  REQUIRE(sentence.Valid());
  REQUIRE(sentence.parameters[0] == "2");
  REQUIRE(sentence.parameters[1] == "12.9");
  REQUIRE(sentence.parameters[2] == "1000");
}

TEST_CASE("reuses sentence object", "[link][parser][sentence]") {
  Parser parser;
  Sentence sentence;
  parser.ParseText(sentence, "$ABC,2,12.9,1000*4B");
  REQUIRE(sentence.parameters.size() == 3);
  REQUIRE(sentence.name == "ABC");
  REQUIRE(sentence.Valid());
  REQUIRE(sentence.parameters[0] == "2");
  REQUIRE(sentence.parameters[1] == "12.9");
  REQUIRE(sentence.parameters[2] == "1000");

  parser.ParseText(sentence, "$PMTK,000,12,1*2C");
  REQUIRE(sentence.parameters.size() == 3);
  REQUIRE(sentence.name == "PMTK");
  REQUIRE(sentence.Valid());
  REQUIRE(sentence.parameters[0] == "000");
  REQUIRE(sentence.parameters[1] == "12");
  REQUIRE(sentence.parameters[2] == "1");
}
