#include <argparse/argparse.hpp>
#include <doctest.hpp>

using doctest::test_suite;

TEST_CASE("Parse negative integer" * test_suite("positional_arguments")) {
  argparse::ArgumentParser program;
  program.add_argument("--verbose", "-v")
      .help("enable verbose logging")
      .default_value(false)
      .implicit_value(true);

  program.add_argument("number").help("Input number").scan<'i', int>();

  program.parse_args({"./main", "-1"});
  REQUIRE(program.get<int>("number") == -1);
}

TEST_CASE("Parse negative integers into a vector" *
          test_suite("positional_arguments")) {
  argparse::ArgumentParser program;
  program.add_argument("--verbose", "-v")
      .help("enable verbose logging")
      .default_value(false)
      .implicit_value(true);

  program.add_argument("number").help("Input number").nargs(3).scan<'i', int>();

  program.parse_args({"./main", "-1", "-2", "3"});
  REQUIRE(program["number"] == std::vector<int>{-1, -2, 3});
}

TEST_CASE("Parse negative float" * test_suite("positional_arguments")) {
  argparse::ArgumentParser program;
  program.add_argument("--verbose", "-v")
      .help("enable verbose logging")
      .default_value(false)
      .implicit_value(true);

  program.add_argument("number").help("Input number").scan<'g', float>();

  program.parse_args({"./main", "-1.0"});
  REQUIRE(program.get<float>("number") == -1.0);
}

TEST_CASE("Parse negative floats into a vector" *
          test_suite("positional_arguments")) {
  argparse::ArgumentParser program;
  program.add_argument("--verbose", "-v")
      .help("enable verbose logging")
      .default_value(false)
      .implicit_value(true);

  program.add_argument("number")
      .help("Input number")
      .nargs(3)
      .scan<'g', double>();

  program.parse_args({"./main", "-1.001", "-2.002", "3.003"});
  REQUIRE(program["number"] == std::vector<double>{-1.001, -2.002, 3.003});
}

TEST_CASE("Parse numbers in E notation" * test_suite("positional_arguments")) {
  argparse::ArgumentParser program;
  program.add_argument("--verbose", "-v")
      .help("enable verbose logging")
      .default_value(false)
      .implicit_value(true);

  program.add_argument("number").help("Input number").scan<'g', double>();

  program.parse_args({"./main", "-1.2e3"});
  REQUIRE(program.get<double>("number") == -1200.0);
}

TEST_CASE("Parse numbers in E notation (capital E)" *
          test_suite("positional_arguments")) {
  argparse::ArgumentParser program;
  program.add_argument("--verbose", "-v")
      .help("enable verbose logging")
      .default_value(false)
      .implicit_value(true);

  program.add_argument("number").help("Input number").scan<'g', double>();

  program.parse_args({"./main", "-1.32E4"});
  REQUIRE(program.get<double>("number") == -13200.0);
}

TEST_CASE("Recognize negative decimal numbers" *
          test_suite("positional_arguments")) {
  argparse::ArgumentParser program("test");
  program.add_argument("positional");

  SUBCASE("zero") { REQUIRE_NOTHROW(program.parse_args({"test", "-0"})); }
  SUBCASE("not a decimal") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-00"}), std::runtime_error);
  }
  SUBCASE("looks like an octal") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-003"}), std::runtime_error);
  }
  SUBCASE("nonzero-digit") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-9"}));
  }
  SUBCASE("nonzero-digit digit-sequence") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-92180"}));
  }
  SUBCASE("zero dot") { REQUIRE_NOTHROW(program.parse_args({"test", "-0."})); }
  SUBCASE("nonzero-digit dot") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-8."}));
  }
  SUBCASE("nonzero-digit digit-sequence dot") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-200."}));
  }
  SUBCASE("integer-part dot") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-003."}));
  }
  SUBCASE("dot digit-sequence") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-.0927"}));
  }
  SUBCASE("not a single dot") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-."}), std::runtime_error);
  }
  SUBCASE("not a single e") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-e"}), std::runtime_error);
  }
  SUBCASE("not dot e") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-.e"}), std::runtime_error);
  }
  SUBCASE("integer-part exponent-part without sign") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-1e32"}));
  }
  SUBCASE("integer-part exponent-part with positive sign") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-1e+32"}));
  }
  SUBCASE("integer-part exponent-part with negative sign") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-00e-0"}));
  }
  SUBCASE("missing mantissa") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-e32"}), std::runtime_error);
  }
  SUBCASE("missing mantissa but with positive sign") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-e+7"}), std::runtime_error);
  }
  SUBCASE("missing mantissa but with negative sign") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-e-1"}), std::runtime_error);
  }
  SUBCASE("nothing after e followed by zero") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-0e"}), std::runtime_error);
  }
  SUBCASE("nothing after e followed by integer-part") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-13e"}), std::runtime_error);
  }
  SUBCASE("integer-part dot exponent-part without sign") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-18.e0"}));
  }
  SUBCASE("integer-part dot exponent-part with positive sign") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-18.e+92"}));
  }
  SUBCASE("integer-part dot exponent-part with negative sign") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-0.e-92"}));
  }
  SUBCASE("nothing after e followed by integer-part dot") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-13.e"}),
                      std::runtime_error);
  }
  SUBCASE("dot digit-sequence exponent-part without sign") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-.023e0"}));
  }
  SUBCASE("dot digit-sequence exponent-part with positive sign") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-.2e+92"}));
  }
  SUBCASE("dot digit-sequence exponent-part with negative sign") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-.71564e-92"}));
  }
  SUBCASE("nothing after e in fractional-part") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-.283e"}),
                      std::runtime_error);
  }
  SUBCASE("exponent-part followed by only a dot") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-.e3"}), std::runtime_error);
  }
  SUBCASE("exponent-part followed by only a dot but with positive sign") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-.e+3"}),
                      std::runtime_error);
  }
  SUBCASE("exponent-part followed by only a dot but with negative sign") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-.e-3"}),
                      std::runtime_error);
  }
  SUBCASE("integer-part dot digit-sequence exponent-part without sign") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-02.023e4000"}));
  }
  SUBCASE("integer-part dot digit-sequence exponent-part with positive sign") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-3.239e+76"}));
  }
  SUBCASE("integer-part dot digit-sequence exponent-part with negative sign") {
    REQUIRE_NOTHROW(program.parse_args({"test", "-238237.0e-2"}));
  }
  SUBCASE("nothing after e") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-3.14e"}),
                      std::runtime_error);
  }
  SUBCASE("nothing after e and positive sign") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-2.17e+"}),
                      std::runtime_error);
  }
  SUBCASE("nothing after e and negative sign") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-13.6e-"}),
                      std::runtime_error);
  }
  SUBCASE("more than one sign present in exponent-part") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-13.6e+-23"}),
                      std::runtime_error);
  }
  SUBCASE("sign at wrong position") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-3.6e23+"}),
                      std::runtime_error);
  }
  SUBCASE("more than one exponent-part") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-3.6e2e9"}),
                      std::runtime_error);
  }
  SUBCASE("more than one fractional-part") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-3.6.3"}),
                      std::runtime_error);
  }
  SUBCASE("number has its own sign") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-+42"}), std::runtime_error);
  }
  SUBCASE("looks like hexadecimal integer") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-0x0"}), std::runtime_error);
  }
  SUBCASE("looks like hexadecimal floating-point") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-0x27.8p1"}),
                      std::runtime_error);
  }
  SUBCASE("looks like hexadecimal floating-point without prefix") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-3.8p1"}),
                      std::runtime_error);
  }
  SUBCASE("Richard's pp-number") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-0x1e+2"}),
                      std::runtime_error);
  }
  SUBCASE("Infinity") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-inf"}), std::runtime_error);
    REQUIRE_THROWS_AS(program.parse_args({"test", "-INFINITY"}),
                      std::runtime_error);
  }
  SUBCASE("NaN") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-nan"}), std::runtime_error);
    REQUIRE_THROWS_AS(program.parse_args({"test", "-NAN"}), std::runtime_error);
  }
}
