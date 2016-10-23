#include "catch/catch.hpp"
#include "cpptoml/cpptoml.h"

#include "deadeye/config.h"
#include "testdata.h"

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

TEST_CASE("throws exception for missing config", "[config]") {
  REQUIRE_THROWS(make_unique<deadeye::Config>("foo"));
}

TEST_CASE("throws exception for malformed config", "[config]") {
  REQUIRE_THROWS_AS(make_unique<deadeye::Config>(kTestDataDir + "/bogus.toml"),
                    cpptoml::parse_exception);
}

TEST_CASE("parses config with no logger", "[config]") {
  auto config = make_unique<deadeye::Config>(kTestDataDir + "/test.toml");
  REQUIRE(config);

  auto table = config->GetTable("logging");
  auto level = table->get_as<std::string>("level");
  REQUIRE(*level == "info");

  table = config->GetTable("camera");
  auto exposure = table->get_as<int>("exposure");
  REQUIRE(*exposure == 10);

  table = config->GetTable("robot");
  auto address = table->get_as<std::string>("address");
  REQUIRE(*address == "127.0.0.1");
  auto port = table->get_as<std::string>("port");
  REQUIRE(*port == "5804");

  table = config->GetTable("target");
  auto hsv_l = table->get_array_of<int64_t>("hsv_lower");
  REQUIRE((*hsv_l)[0] == 86);
  REQUIRE((*hsv_l)[1] == 100);
  REQUIRE((*hsv_l)[2] == 100);
  auto hsv_u = table->get_array_of<int64_t>("hsv_upper");
  REQUIRE((*hsv_u)[0] == 96);
  REQUIRE((*hsv_u)[1] == 255);
  REQUIRE((*hsv_u)[2] == 255);
  auto blur_size = table->get_array_of<int64_t>("blur_size");
  REQUIRE((*blur_size)[0] == 3);
  REQUIRE((*blur_size)[1] == 3);
  auto min_perimeter = table->get_as<int>("min_perimeter");
  REQUIRE(*min_perimeter == 250);
  auto closing = table->get_as<bool>("closing");
  REQUIRE(*closing);
}
