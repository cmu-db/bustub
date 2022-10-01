#include <argparse/argparse.hpp>
#include <doctest.hpp>

using doctest::test_suite;

TEST_CASE("User-supplied argument" * test_suite("is_used")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--dir").default_value(std::string("/"));
  program.parse_args({"test", "--dir", "/home/user"});
  REQUIRE(program.get("--dir") == "/home/user");
  REQUIRE(program.is_used("--dir") == true);
}

TEST_CASE("Not user-supplied argument" * test_suite("is_used")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--dir").default_value(std::string("/"));
  program.parse_args({"test"});
  REQUIRE(program.get("--dir") == "/");
  REQUIRE(program.is_used("--dir") == false);
}
