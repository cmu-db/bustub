#include <argparse/argparse.hpp>
#include <doctest.hpp>
#include <set>

using doctest::test_suite;

TEST_CASE("Test bool representation" * test_suite("repr")) {
  REQUIRE(argparse::details::repr(true) == "true");
  REQUIRE(argparse::details::repr(false) == "false");
}

TEST_CASE_TEMPLATE("Test built-in int types representation" *
                       test_suite("repr"),
                   T, char, short, int, long long, unsigned char, unsigned,
                   unsigned long long) {
  std::stringstream ss;
  T v = 42;
  ss << v;
  REQUIRE(argparse::details::repr(v) == ss.str());
}

TEST_CASE_TEMPLATE("Test built-in float types representation" *
                       test_suite("repr"),
                   T, float, double, long double) {
  std::stringstream ss;
  T v = static_cast<T>(0.3333333333);
  ss << v;
  REQUIRE(argparse::details::repr(v) == ss.str());
}

TEST_CASE_TEMPLATE("Test container representation" * test_suite("repr"), T,
                   std::vector<int>, std::list<int>, std::set<int>) {
  T empty;
  T one = {42};
  T small = {1, 2, 3};
  T big = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

  REQUIRE(argparse::details::repr(empty) == "{}");
  REQUIRE(argparse::details::repr(one) == "{42}");
  REQUIRE(argparse::details::repr(small) == "{1 2 3}");
  REQUIRE(argparse::details::repr(big) == "{1 2 3 4...15}");
}

TEST_CASE_TEMPLATE("Test string representation" * test_suite("repr"), T,
                   char const *, std::string, std::string_view) {
  T empty = "";
  T str = "A A A#";

  REQUIRE(argparse::details::repr(empty) == "\"\"");
  REQUIRE(argparse::details::repr(str) == "\"A A A#\"");
}

TEST_CASE("Test unknown representation" * test_suite("repr")) {
  struct TestClass {};
  REQUIRE(argparse::details::repr(TestClass{}) == "<not representable>");
}
