#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace std;

TEST_CASE("Core::Resources") {
  const string input{"A"};
  REQUIRE(input == "A");
}

