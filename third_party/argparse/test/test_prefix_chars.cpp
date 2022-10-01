#include <argparse/argparse.hpp>
#include <cmath>
#include <doctest.hpp>

using doctest::test_suite;

TEST_CASE("Parse with custom prefix chars" * test_suite("prefix_chars")) {
  argparse::ArgumentParser program("test");
  program.set_prefix_chars("-+");
  program.add_argument("+f");
  program.add_argument("++bar");
  program.parse_args({"test", "+f", "X", "++bar", "Y"});
  REQUIRE(program.get("+f") == "X");
  REQUIRE(program.get("++bar") == "Y");
}

TEST_CASE("Parse with custom Windows-style prefix chars" *
          test_suite("prefix_chars")) {
  argparse::ArgumentParser program("dir");
  program.set_prefix_chars("/");
  program.add_argument("/A").nargs(1);
  program.add_argument("/B").default_value(false).implicit_value(true);
  program.add_argument("/C").default_value(false).implicit_value(true);
  program.parse_args({"dir", "/A", "D", "/B", "/C"});
  REQUIRE(program.get("/A") == "D");
  REQUIRE(program.get<bool>("/B") == true);
}

TEST_CASE("Parse with custom Windows-style prefix chars and assign chars" *
          test_suite("prefix_chars")) {
  argparse::ArgumentParser program("dir");
  program.set_prefix_chars("/");
  program.set_assign_chars(":=");
  program.add_argument("/A").nargs(1);
  program.add_argument("/B").nargs(1);
  program.add_argument("/C").default_value(false).implicit_value(true);
  program.parse_args({"dir", "/A:D", "/B=Boo", "/C"});
  REQUIRE(program.get("/A") == "D");
  REQUIRE(program.get("/B") == "Boo");
  REQUIRE(program.get<bool>("/C") == true);
}