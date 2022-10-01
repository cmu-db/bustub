#include <argparse/argparse.hpp>
#include <doctest.hpp>
#include <iostream>
using doctest::test_suite;

TEST_CASE("Basic --value=value" * test_suite("equals_form")) {
  argparse::ArgumentParser parser("test");
  parser.add_argument("--long");
  parser.parse_args({"test", "--long=value"});
  std::string result{parser.get("--long")};
  REQUIRE(result == "value");
}

TEST_CASE("Fallback = in regular option name" * test_suite("equals_form")) {
  argparse::ArgumentParser parser("test");
  parser.add_argument("--long=mislead");
  parser.parse_args({"test", "--long=mislead", "value"});
  std::string result{parser.get("--long=mislead")};
  REQUIRE(result == "value");
}

TEST_CASE("Duplicate =-named and standard" * test_suite("equals_form")) {
  argparse::ArgumentParser parser("test");
  parser.add_argument("--long=mislead");
  parser.add_argument("--long").default_value(std::string{"NO_VALUE"});
  parser.parse_args({"test", "--long=mislead", "value"});
  std::string result{parser.get("--long=mislead")};
  REQUIRE(result == "value");
  std::string result2{parser.get("--long")};
  REQUIRE(result2 == "NO_VALUE");
}

TEST_CASE("Basic --value=value with nargs(2)" * test_suite("equals_form")) {
  argparse::ArgumentParser parser("test");
  parser.add_argument("--long").nargs(2);
  parser.parse_args({"test", "--long=value1", "value2"});
  REQUIRE((parser.get<std::vector<std::string>>("--long") ==
           std::vector<std::string>{"value1", "value2"}));
}