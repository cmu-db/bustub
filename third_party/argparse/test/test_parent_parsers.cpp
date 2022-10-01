#include <argparse/argparse.hpp>
#include <doctest.hpp>

using doctest::test_suite;

TEST_CASE("Add parent parsers" * test_suite("parent_parsers")) {
  argparse::ArgumentParser parent_parser("main");
  parent_parser.add_argument("--verbose")
      .default_value(false)
      .implicit_value(true);

  argparse::ArgumentParser child_parser("foo");
  child_parser.add_parents(parent_parser);
  child_parser.parse_args({"./main", "--verbose"});
  REQUIRE(child_parser["--verbose"] == true);
  REQUIRE(parent_parser["--verbose"] == false);
}

TEST_CASE("Add parent to multiple parent parsers" *
          test_suite("parent_parsers")) {
  argparse::ArgumentParser parent_parser("main");
  parent_parser.add_argument("--parent").default_value(0).scan<'i', int>();

  argparse::ArgumentParser foo_parser("foo");
  foo_parser.add_argument("foo");
  foo_parser.add_parents(parent_parser);
  foo_parser.parse_args({"./main", "--parent", "2", "XXX"});
  REQUIRE(foo_parser["--parent"] == 2);
  REQUIRE(foo_parser["foo"] == std::string("XXX"));
  REQUIRE(parent_parser["--parent"] == 0);

  argparse::ArgumentParser bar_parser("bar");
  bar_parser.add_argument("--bar");
  bar_parser.parse_args({"./main", "--bar", "YYY"});
  REQUIRE(bar_parser["--bar"] == std::string("YYY"));
}
