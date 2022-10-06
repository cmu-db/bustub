#include <argparse/argparse.hpp>
#include <doctest.hpp>

using doctest::test_suite;

TEST_CASE("Simplest .append" * test_suite("append")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--dir").append();
  program.parse_args({"test", "--dir", "./Docs"});
  std::string result{program.get("--dir")};
  REQUIRE(result == "./Docs");
}

TEST_CASE("Two parameter .append" * test_suite("append")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--dir").append();
  program.parse_args({"test", "--dir", "./Docs", "--dir", "./Src"});
  auto result{program.get<std::vector<std::string>>("--dir")};
  REQUIRE(result.at(0) == "./Docs");
  REQUIRE(result.at(1) == "./Src");
}

TEST_CASE("Two int .append" * test_suite("append")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--factor").append().scan<'i', int>();
  program.parse_args({"test", "--factor", "2", "--factor", "5"});
  auto result{program.get<std::vector<int>>("--factor")};
  REQUIRE(result.at(0) == 2);
  REQUIRE(result.at(1) == 5);
}

TEST_CASE("Default value with .append" * test_suite("append")) {
  std::vector<std::string> expected{"./Src", "./Imgs"};

  argparse::ArgumentParser program("test");
  program.add_argument("--dir").default_value(expected).append();
  program.parse_args({"test"});
  auto result{program.get<std::vector<std::string>>("--dir")};
  REQUIRE(result == expected);
}
