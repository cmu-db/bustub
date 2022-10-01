#include <argparse/argparse.hpp>
#include <doctest.hpp>

using doctest::test_suite;

TEST_CASE("Parse unknown optional and positional arguments without exceptions" *
          test_suite("parse_known_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--foo").implicit_value(true).default_value(false);
  program.add_argument("bar");

  SUBCASE("Parse unknown optional and positional arguments") {
    auto unknown_args =
        program.parse_known_args({"test", "--foo", "--badger", "BAR", "spam"});
    REQUIRE((unknown_args == std::vector<std::string>{"--badger", "spam"}));
    REQUIRE(program.get<bool>("--foo") == true);
    REQUIRE(program.get<std::string>("bar") == std::string{"BAR"});
  }

  SUBCASE("Parse unknown compound arguments") {
    auto unknown_args = program.parse_known_args({"test", "-jc", "BAR"});
    REQUIRE((unknown_args == std::vector<std::string>{"-jc"}));
    REQUIRE(program.get<bool>("--foo") == false);
    REQUIRE(program.get<std::string>("bar") == std::string{"BAR"});
  }
}

TEST_CASE("Parse unknown optional and positional arguments in subparsers "
          "without exceptions" *
          test_suite("parse_known_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--output");

  argparse::ArgumentParser command_1("add");
  command_1.add_argument("file").nargs(2);

  argparse::ArgumentParser command_2("clean");
  command_2.add_argument("--fullclean")
      .default_value(false)
      .implicit_value(true);

  program.add_subparser(command_1);
  program.add_subparser(command_2);

  SUBCASE("Parse unknown optional argument") {
    auto unknown_args =
        program.parse_known_args({"test", "add", "--badger", "BAR", "spam"});
    REQUIRE(program.is_subcommand_used("add") == true);
    REQUIRE((command_1.get<std::vector<std::string>>("file") ==
             std::vector<std::string>{"BAR", "spam"}));
    REQUIRE((unknown_args == std::vector<std::string>{"--badger"}));
  }

  SUBCASE("Parse unknown positional argument") {
    auto unknown_args =
        program.parse_known_args({"test", "add", "FOO", "BAR", "spam"});
    REQUIRE(program.is_subcommand_used("add") == true);
    REQUIRE((command_1.get<std::vector<std::string>>("file") ==
             std::vector<std::string>{"FOO", "BAR"}));
    REQUIRE((unknown_args == std::vector<std::string>{"spam"}));
  }

  SUBCASE("Parse unknown positional and optional arguments") {
    auto unknown_args = program.parse_known_args(
        {"test", "add", "--verbose", "FOO", "5", "BAR", "-jn", "spam"});
    REQUIRE(program.is_subcommand_used("add") == true);
    REQUIRE((command_1.get<std::vector<std::string>>("file") ==
             std::vector<std::string>{"FOO", "5"}));
    REQUIRE((unknown_args ==
             std::vector<std::string>{"--verbose", "BAR", "-jn", "spam"}));
  }

  SUBCASE("Parse unknown positional and optional arguments 2") {
    auto unknown_args =
        program.parse_known_args({"test", "clean", "--verbose", "FOO", "5",
                                  "BAR", "--fullclean", "-jn", "spam"});
    REQUIRE(program.is_subcommand_used("clean") == true);
    REQUIRE(command_2.get<bool>("--fullclean") == true);
    REQUIRE((unknown_args == std::vector<std::string>{"--verbose", "FOO", "5",
                                                      "BAR", "-jn", "spam"}));
  }
}