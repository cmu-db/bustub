#include <argparse/argparse.hpp>
#include <doctest.hpp>

using doctest::test_suite;

TEST_CASE("Getting a simple argument" * test_suite("ArgumentParser::get")) {
  argparse::ArgumentParser program("test");
  program.add_argument("-s", "--stuff");
  REQUIRE_NOTHROW(program.parse_args({"test", "-s", "./src"}));
  REQUIRE(program.get("--stuff") == "./src");
}

TEST_CASE("Skipped call to parse_args" * test_suite("ArgumentParser::get")) {
  argparse::ArgumentParser program("test");
  program.add_argument("stuff");
  REQUIRE_THROWS_WITH_AS(program.get("stuff"),
                         "Nothing parsed, no arguments are available.",
                         std::logic_error);
}

TEST_CASE("Missing argument" * test_suite("ArgumentParser::get")) {
  argparse::ArgumentParser program("test");
  program.add_argument("-s", "--stuff");
  REQUIRE_NOTHROW(program.parse_args({"test"}));
  REQUIRE_THROWS_WITH_AS(program.get("--stuff"),
                         "No value provided for '--stuff'.", std::logic_error);
}

TEST_CASE("Implicit argument" * test_suite("ArgumentParser::get")) {
  argparse::ArgumentParser program("test");
  program.add_argument("-s", "--stuff").nargs(1);
  REQUIRE_NOTHROW(program.parse_args({"test"}));
  REQUIRE_THROWS_WITH_AS(program.get("--stuff"),
                         "No value provided for '--stuff'.", std::logic_error);
}
