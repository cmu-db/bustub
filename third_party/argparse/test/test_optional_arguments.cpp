#include <argparse/argparse.hpp>
#include <doctest.hpp>

using doctest::test_suite;

TEST_CASE("Parse toggle arguments with default value" *
          test_suite("optional_arguments")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--verbose", "-v")
      .default_value(false)
      .implicit_value(true);

  program.parse_args({"./test.exe"});
  REQUIRE(program.get<bool>("--verbose") == false);
  REQUIRE(program["--verbose"] == false);
}

TEST_CASE("Argument '-' is not an optional argument" *
          test_suite("optional_arguments")) {
  argparse::ArgumentParser program("test");
  program.add_argument("input");
  program.parse_args({"./test.exe", "-"});
  REQUIRE(program.get<std::string>("input") == "-");
}

TEST_CASE("Argument '-' is not an optional argument but '-l' is" *
          test_suite("optional_arguments")) {
  argparse::ArgumentParser program("test");
  program.add_argument("-l").default_value(false).implicit_value(true);
  program.add_argument("input");
  program.parse_args({"./test.exe", "-l", "-"});
  REQUIRE(program.get<bool>("-l") == true);
  REQUIRE(program.get<std::string>("input") == "-");
}

TEST_CASE("Argument '-l' is an optional argument but '-' is not" *
          test_suite("optional_arguments")) {
  argparse::ArgumentParser program("test");
  program.add_argument("-l").default_value(false).implicit_value(true);
  program.add_argument("input");
  program.parse_args({"./test.exe", "-", "-l"});
  REQUIRE(program.get<bool>("-l") == true);
  REQUIRE(program.get<std::string>("input") == "-");
}

TEST_CASE("Parse toggle arguments with implicit value" *
          test_suite("optional_arguments")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--verbose").default_value(false).implicit_value(true);

  program.parse_args({"./test.exe", "--verbose"});
  REQUIRE(program.get<bool>("--verbose") == true);
  REQUIRE(program["--verbose"] == true);
  REQUIRE(program["--verbose"] != false);
}

TEST_CASE("Parse multiple toggle arguments with implicit values" *
          test_suite("optional_arguments")) {
  argparse::ArgumentParser program("test");
  program.add_argument("-a").default_value(false).implicit_value(true);

  program.add_argument("-u").default_value(false).implicit_value(true);

  program.add_argument("-x").default_value(false).implicit_value(true);

  program.parse_args({"./test.exe", "-a", "-x"});
  REQUIRE(program.get<bool>("-a") == true);
  REQUIRE(program.get<bool>("-u") == false);
  REQUIRE(program.get<bool>("-x") == true);
}

TEST_CASE("Parse optional arguments of many values" *
          test_suite("optional_arguments")) {
  GIVEN("a program that accepts an optional argument of many values") {
    argparse::ArgumentParser program("test");
    program.add_argument("-i").remaining().scan<'i', int>();

    WHEN("provided no argument") {
      THEN("the program accepts it but gets nothing") {
        REQUIRE_NOTHROW(program.parse_args({"test"}));
        REQUIRE_THROWS_AS(program.get<std::vector<int>>("-i"),
                          std::logic_error);
      }
    }

    WHEN("provided remaining arguments follow the option") {
      program.parse_args({"test", "-i", "-42", "8", "100", "300"});

      THEN("the optional parameter consumes all of them") {
        auto inputs = program.get<std::vector<int>>("-i");
        REQUIRE(inputs.size() == 4);
        REQUIRE(inputs[0] == -42);
        REQUIRE(inputs[1] == 8);
        REQUIRE(inputs[2] == 100);
        REQUIRE(inputs[3] == 300);
      }
    }
  }
}

TEST_CASE("Parse 2 optional arguments of many values" *
          test_suite("optional_arguments")) {
  GIVEN("a program that accepts 2 optional arguments of many values") {
    argparse::ArgumentParser program("test");
    program.add_argument("-i")
        .nargs(argparse::nargs_pattern::any)
        .scan<'i', int>();
    program.add_argument("-s").nargs(argparse::nargs_pattern::any);

    WHEN("provided no argument") {
      THEN("the program accepts it and gets empty container") {
        REQUIRE_NOTHROW(program.parse_args({"test"}));
        auto i = program.get<std::vector<int>>("-i");
        REQUIRE(i.size() == 0);

        auto s = program.get<std::vector<std::string>>("-s");
        REQUIRE(s.size() == 0);
      }
    }

    WHEN("provided 2 options with many arguments") {
      program.parse_args({"test", "-i", "-42", "8", "100", "300", "-s", "ok",
                          "this", "works"});

      THEN("the optional parameter consumes each arguments") {
        auto i = program.get<std::vector<int>>("-i");
        REQUIRE(i.size() == 4);
        REQUIRE(i[0] == -42);
        REQUIRE(i[1] == 8);
        REQUIRE(i[2] == 100);
        REQUIRE(i[3] == 300);

        auto s = program.get<std::vector<std::string>>("-s");
        REQUIRE(s.size() == 3);
        REQUIRE(s[0] == "ok");
        REQUIRE(s[1] == "this");
        REQUIRE(s[2] == "works");
      }
    }
  }
}

TEST_CASE("Parse an optional argument of many values"
          " and a positional argument of many values" *
          test_suite("optional_arguments")) {
  GIVEN("a program that accepts an optional argument of many values"
        " and a positional argument of many values") {
    argparse::ArgumentParser program("test");
    program.add_argument("-s").nargs(argparse::nargs_pattern::any);
    program.add_argument("input").nargs(argparse::nargs_pattern::any);

    WHEN("provided no argument") {
      program.parse_args({"test"});
      THEN("the program accepts it and gets empty containers") {
        auto s = program.get<std::vector<std::string>>("-s");
        REQUIRE(s.size() == 0);

        auto input = program.get<std::vector<std::string>>("input");
        REQUIRE(input.size() == 0);
      }
    }

    WHEN("provided many arguments followed by an option with many arguments") {
      program.parse_args({"test", "foo", "bar", "-s", "ok", "this", "works"});

      THEN("the parameters consume each arguments") {
        auto s = program.get<std::vector<std::string>>("-s");
        REQUIRE(s.size() == 3);
        REQUIRE(s[0] == "ok");
        REQUIRE(s[1] == "this");
        REQUIRE(s[2] == "works");

        auto input = program.get<std::vector<std::string>>("input");
        REQUIRE(input.size() == 2);
        REQUIRE(input[0] == "foo");
        REQUIRE(input[1] == "bar");
      }
    }
  }
}

TEST_CASE("Parse arguments of different types" *
          test_suite("optional_arguments")) {
  using namespace std::literals;

  argparse::ArgumentParser program("test");
  program.add_argument("--this-argument-is-longer-than-any-sso-buffer-that-"
                       "makes-sense-unless-your-cache-line-is-this-long"s);

  REQUIRE_NOTHROW(program.parse_args({"test"}));

  program.add_argument("-string"s, "-string-view"sv, "-builtin")
      .default_value(false)
      .implicit_value(true);

  program.parse_args({"test", "-string-view"});
  REQUIRE(program["-string"sv] == true);
  REQUIRE(program["-string-view"] == true);
  REQUIRE(program["-builtin"s] == true);
}
