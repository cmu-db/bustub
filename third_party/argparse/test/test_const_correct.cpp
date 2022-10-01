#include <argparse/argparse.hpp>
#include <doctest.hpp>

using doctest::test_suite;

TEST_CASE("ArgumentParser is const-correct after construction and parsing" *
          test_suite("value_semantics")) {
  GIVEN("a parser") {
    argparse::ArgumentParser parser("test");
    parser.add_argument("--foo", "-f").help("I am foo");
    parser.add_description("A description");
    parser.add_epilog("An epilog");

    WHEN("becomes const-qualified") {
      parser.parse_args({"./main", "--foo", "baz"});
      const auto const_parser = std::move(parser);

      THEN("only const methods are accessible") {
        REQUIRE(const_parser.help().str().size() > 0);
        REQUIRE(const_parser.present<std::string>("--foo"));
        REQUIRE(const_parser.is_used("-f"));
        REQUIRE(const_parser.get("-f") == "baz");
        REQUIRE(const_parser["-f"] == std::string("baz"));
      }
    }
  }
}
