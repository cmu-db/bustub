#include <argparse/argparse.hpp>
#include <cmath>
#include <doctest.hpp>

using doctest::test_suite;

TEST_CASE("Parse positional arguments" * test_suite("positional_arguments")) {
  argparse::ArgumentParser program("test");
  program.add_argument("input");
  program.add_argument("output");
  program.parse_args({"test", "rocket.mesh", "thrust_profile.csv"});
  REQUIRE(program.get("input") == "rocket.mesh");
  REQUIRE(program.get("output") == "thrust_profile.csv");
}

TEST_CASE("Missing expected positional argument" *
          test_suite("positional_arguments")) {
  argparse::ArgumentParser program("test");
  program.add_argument("input");
  REQUIRE_THROWS_WITH_AS(program.parse_args({"test"}),
                         "1 argument(s) expected. 0 provided.",
                         std::runtime_error);
}

TEST_CASE("Parse positional arguments with fixed nargs" *
          test_suite("positional_arguments")) {
  argparse::ArgumentParser program("test");
  program.add_argument("input");
  program.add_argument("output").nargs(2);
  program.parse_args(
      {"test", "rocket.mesh", "thrust_profile.csv", "output.mesh"});
  REQUIRE(program.get("input") == "rocket.mesh");
  auto outputs = program.get<std::vector<std::string>>("output");
  REQUIRE(outputs.size() == 2);
  REQUIRE(outputs[0] == "thrust_profile.csv");
  REQUIRE(outputs[1] == "output.mesh");
}

TEST_CASE("Parse positional arguments with optional arguments" *
          test_suite("positional_arguments")) {
  argparse::ArgumentParser program("test");
  program.add_argument("input");
  program.add_argument("output").nargs(2);
  program.add_argument("--num_iterations").scan<'i', int>();
  program.parse_args({"test", "rocket.mesh", "--num_iterations", "15",
                      "thrust_profile.csv", "output.mesh"});
  REQUIRE(program.get<int>("--num_iterations") == 15);
  REQUIRE(program.get("input") == "rocket.mesh");
  auto outputs = program.get<std::vector<std::string>>("output");
  REQUIRE(outputs.size() == 2);
  REQUIRE(outputs[0] == "thrust_profile.csv");
  REQUIRE(outputs[1] == "output.mesh");
}

TEST_CASE("Parse positional arguments with optional arguments in the middle" *
          test_suite("positional_arguments")) {
  argparse::ArgumentParser program("test");
  program.add_argument("input");
  program.add_argument("output").nargs(2);
  program.add_argument("--num_iterations").scan<'i', int>();
  REQUIRE_THROWS(
      program.parse_args({"test", "rocket.mesh", "thrust_profile.csv",
                          "--num_iterations", "15", "output.mesh"}));
}

TEST_CASE("Parse positional nargs=1..2 arguments" *
          test_suite("positional_arguments")) {
  GIVEN("a program that accepts an optional argument and nargs=1..2 positional "
        "arguments") {
    argparse::ArgumentParser program("test");
    program.add_argument("-o");
    program.add_argument("input").nargs(1, 2);

    WHEN("provided no argument") {
      THEN("the program does not accept it") {
        REQUIRE_THROWS(program.parse_args({"test"}));
      }
    }

    WHEN("provided 1 argument") {
      THEN("the program accepts it") {
        REQUIRE_NOTHROW(program.parse_args({"test", "a.c"}));

        auto inputs = program.get<std::vector<std::string>>("input");
        REQUIRE(inputs.size() == 1);
        REQUIRE(inputs[0] == "a.c");
      }
    }

    WHEN("provided 2 arguments") {
      THEN("the program accepts it") {
        REQUIRE_NOTHROW(program.parse_args({"test", "a.c", "b.c"}));

        auto inputs = program.get<std::vector<std::string>>("input");
        REQUIRE(inputs.size() == 2);
        REQUIRE(inputs[0] == "a.c");
        REQUIRE(inputs[1] == "b.c");
      }
    }

    WHEN("provided 3 arguments") {
      THEN("the program does not accept it") {
        REQUIRE_THROWS(program.parse_args({"test", "a.c", "b.c", "main.c"}));
      }
    }

    WHEN("provided an optional followed by positional arguments") {
      program.parse_args({"test", "-o", "a.out", "a.c", "b.c"});

      THEN("the optional parameter consumes an argument") {
        using namespace std::literals;
        REQUIRE(program["-o"] == "a.out"s);

        auto inputs = program.get<std::vector<std::string>>("input");
        REQUIRE(inputs.size() == 2);
        REQUIRE(inputs[0] == "a.c");
        REQUIRE(inputs[1] == "b.c");
      }
    }

    WHEN("provided an optional preceded by positional arguments") {
      program.parse_args({"test", "a.c", "b.c", "-o", "a.out"});

      THEN("the optional parameter consumes an argument") {
        using namespace std::literals;
        REQUIRE(program["-o"] == "a.out"s);

        auto inputs = program.get<std::vector<std::string>>("input");
        REQUIRE(inputs.size() == 2);
        REQUIRE(inputs[0] == "a.c");
        REQUIRE(inputs[1] == "b.c");
      }
    }

    WHEN("provided an optional in between positional arguments") {
      THEN("the program does not accept it") {
        REQUIRE_THROWS(
            program.parse_args({"test", "a.c", "-o", "a.out", "b.c"}));
      }
    }
  }
}

TEST_CASE("Parse positional nargs=ANY arguments" *
          test_suite("positional_arguments")) {
  GIVEN("a program that accepts an optional argument and nargs=ANY positional "
        "arguments") {
    argparse::ArgumentParser program("test");
    program.add_argument("-o");
    program.add_argument("input").nargs(argparse::nargs_pattern::any);

    WHEN("provided no argument") {
      THEN("the program accepts it and gets empty container") {
        REQUIRE_NOTHROW(program.parse_args({"test"}));

        auto inputs = program.get<std::vector<std::string>>("input");
        REQUIRE(inputs.size() == 0);
      }
    }

    WHEN("provided an optional followed by positional arguments") {
      program.parse_args({"test", "-o", "a.out", "a.c", "b.c", "main.c"});

      THEN("the optional parameter consumes an argument") {
        using namespace std::literals;
        REQUIRE(program["-o"] == "a.out"s);

        auto inputs = program.get<std::vector<std::string>>("input");
        REQUIRE(inputs.size() == 3);
        REQUIRE(inputs[0] == "a.c");
        REQUIRE(inputs[1] == "b.c");
        REQUIRE(inputs[2] == "main.c");
      }
    }

    WHEN("provided an optional preceded by positional arguments") {
      program.parse_args({"test", "a.c", "b.c", "main.c", "-o", "a.out"});

      THEN("the optional parameter consumes an argument") {
        using namespace std::literals;
        REQUIRE(program["-o"] == "a.out"s);

        auto inputs = program.get<std::vector<std::string>>("input");
        REQUIRE(inputs.size() == 3);
        REQUIRE(inputs[0] == "a.c");
        REQUIRE(inputs[1] == "b.c");
        REQUIRE(inputs[2] == "main.c");
      }
    }
  }
}

TEST_CASE("Parse remaining arguments deemed positional" *
          test_suite("positional_arguments")) {
  GIVEN("a program that accepts an optional argument and remaining arguments") {
    argparse::ArgumentParser program("test");
    program.add_argument("-o");
    program.add_argument("input").remaining();

    WHEN("provided no argument") {
      THEN("the program accepts it but gets nothing") {
        REQUIRE_NOTHROW(program.parse_args({"test"}));
        REQUIRE_THROWS_AS(program.get<std::vector<std::string>>("input"),
                          std::logic_error);
      }
    }

    WHEN("provided an optional followed by remaining arguments") {
      program.parse_args({"test", "-o", "a.out", "a.c", "b.c", "main.c"});

      THEN("the optional parameter consumes an argument") {
        using namespace std::literals;
        REQUIRE(program["-o"] == "a.out"s);

        auto inputs = program.get<std::vector<std::string>>("input");
        REQUIRE(inputs.size() == 3);
        REQUIRE(inputs[0] == "a.c");
        REQUIRE(inputs[1] == "b.c");
        REQUIRE(inputs[2] == "main.c");
      }
    }

    WHEN("provided remaining arguments including optional arguments") {
      program.parse_args({"test", "a.c", "b.c", "main.c", "-o", "a.out"});

      THEN("the optional argument is deemed remaining") {
        REQUIRE_THROWS_AS(program.get("-o"), std::logic_error);

        auto inputs = program.get<std::vector<std::string>>("input");
        REQUIRE(inputs.size() == 5);
        REQUIRE(inputs[0] == "a.c");
        REQUIRE(inputs[1] == "b.c");
        REQUIRE(inputs[2] == "main.c");
        REQUIRE(inputs[3] == "-o");
        REQUIRE(inputs[4] == "a.out");
      }
    }
  }
}

TEST_CASE("Reversed order nargs is not allowed" *
          test_suite("positional_arguments")) {
  argparse::ArgumentParser program("test");
  REQUIRE_THROWS_AS(program.add_argument("output").nargs(2, 1),
                    std::logic_error);
}

TEST_CASE("Square a number" * test_suite("positional_arguments")) {
  argparse::ArgumentParser program;
  program.add_argument("--verbose", "-v")
      .help("enable verbose logging")
      .default_value(false)
      .implicit_value(true);

  program.add_argument("square")
      .help("display a square of a given number")
      .action(
          [](const std::string &value) { return pow(std::stoi(value), 2); });

  program.parse_args({"./main", "15"});
  REQUIRE(program.get<double>("square") == 225);
}
