#include <argparse/argparse.hpp>
#include <doctest.hpp>

using doctest::test_suite;

TEST_CASE("ArgumentParser is MoveConstructible and MoveAssignable" *
          test_suite("value_semantics")) {
  GIVEN("a parser that has two arguments") {
    argparse::ArgumentParser parser("test");
    parser.add_argument("foo");
    parser.add_argument("-f");

    WHEN("move construct a new parser from it") {
      auto new_parser = std::move(parser);

      THEN("the old parser replaces the new parser") {
        new_parser.parse_args({"test", "bar", "-f", "nul"});

        REQUIRE(new_parser.get("foo") == "bar");
        REQUIRE(new_parser.get("-f") == "nul");
      }
    }

    WHEN("move assign a parser prvalue to it") {
      parser = argparse::ArgumentParser("test");

      THEN("the old parser is replaced") {
        REQUIRE_THROWS_AS(parser.parse_args({"test", "bar"}),
                          std::runtime_error);
        REQUIRE_THROWS_AS(parser.parse_args({"test", "-f", "nul"}),
                          std::runtime_error);
        REQUIRE_NOTHROW(parser.parse_args({"test"}));
      }
    }
  }
}

TEST_CASE("ArgumentParser is CopyConstructible and CopyAssignable" *
          test_suite("value_semantics")) {
  GIVEN("a parser that has two arguments") {
    argparse::ArgumentParser parser("test");
    parser.add_argument("foo");
    parser.add_argument("-f");

    WHEN("copy construct a new parser from it") {
      auto new_parser = parser;

      THEN("the new parser has the old parser's capability") {
        new_parser.parse_args({"test", "bar", "-f", "nul"});

        REQUIRE(new_parser.get("foo") == "bar");
        REQUIRE(new_parser.get("-f") == "nul");

        AND_THEN("but does not share states with the old parser") {
          REQUIRE_THROWS_AS(parser.get("foo"), std::logic_error);
          REQUIRE_THROWS_AS(parser.get("-f"), std::logic_error);
        }
      }

      AND_THEN("the old parser works as a distinct copy") {
        new_parser.parse_args({"test", "toe", "-f", "/"});

        REQUIRE(new_parser.get("foo") == "toe");
        REQUIRE(new_parser.get("-f") == "/");
      }
    }

    WHEN("copy assign a parser lvalue to it") {
      argparse::ArgumentParser optional_parser("test");
      optional_parser.add_argument("-g");
      parser = optional_parser;

      THEN("the old parser is replaced") {
        REQUIRE_THROWS_AS(parser.parse_args({"test", "bar"}),
                          std::runtime_error);
        REQUIRE_THROWS_AS(parser.parse_args({"test", "-f", "nul"}),
                          std::runtime_error);
        REQUIRE_NOTHROW(parser.parse_args({"test"}));
        REQUIRE_NOTHROW(parser.parse_args({"test", "-g", "nul"}));

        AND_THEN("but does not share states with the other parser") {
          REQUIRE(parser.get("-g") == "nul");
          REQUIRE_THROWS_AS(optional_parser.get("-g"), std::logic_error);
        }
      }

      AND_THEN("the other parser works as a distinct copy") {
        REQUIRE_NOTHROW(optional_parser.parse_args({"test"}));
        REQUIRE_NOTHROW(optional_parser.parse_args({"test", "-g", "nul"}));
        REQUIRE_THROWS_AS(
            optional_parser.parse_args({"test", "bar", "-g", "nul"}),
            std::runtime_error);
      }
    }
  }
}
