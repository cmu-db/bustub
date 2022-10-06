#include <argparse/argparse.hpp>
#include <doctest.hpp>
#include <stdint.h>

using doctest::test_suite;

TEST_CASE_TEMPLATE("Parse a decimal integer argument" * test_suite("scan"), T,
                   int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t,
                   uint32_t, uint64_t) {
  argparse::ArgumentParser program("test");
  program.add_argument("-n").scan<'d', T>();

  SUBCASE("zero") {
    program.parse_args({"test", "-n", "0"});
    REQUIRE(program.get<T>("-n") == 0);
  }

  SUBCASE("non-negative") {
    program.parse_args({"test", "-n", "5"});
    REQUIRE(program.get<T>("-n") == 5);
  }

  SUBCASE("negative") {
    if constexpr (std::is_signed_v<T>) {
      program.parse_args({"test", "-n", "-128"});
      REQUIRE(program.get<T>("-n") == -128);
    } else {
      REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "-135"}),
                        std::invalid_argument);
    }
  }

  SUBCASE("left-padding is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", " 32"}),
                      std::invalid_argument);
  }

  SUBCASE("right-padding is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "12 "}),
                      std::invalid_argument);
  }

  SUBCASE("plus sign is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "+12"}),
                      std::invalid_argument);
  }

  SUBCASE("does not fit") {
    REQUIRE_THROWS_AS(
        program.parse_args({"test", "-n", "987654321987654321987654321"}),
        std::range_error);
  }
}

TEST_CASE_TEMPLATE("Parse an octal integer argument" * test_suite("scan"), T,
                   uint8_t, uint16_t, uint32_t, uint64_t) {
  argparse::ArgumentParser program("test");
  program.add_argument("-n").scan<'o', T>();

  SUBCASE("zero") {
    program.parse_args({"test", "-n", "0"});
    REQUIRE(program.get<T>("-n") == 0);
  }

  SUBCASE("with octal base") {
    program.parse_args({"test", "-n", "066"});
    REQUIRE(program.get<T>("-n") == 066);
  }

  SUBCASE("minus sign produces an optional argument") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "-003"}),
                      std::runtime_error);
  }

  SUBCASE("plus sign is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "+012"}),
                      std::invalid_argument);
  }

  SUBCASE("does not fit") {
    REQUIRE_THROWS_AS(
        program.parse_args({"test", "-n", "02000000000000000000001"}),
        std::range_error);
  }
}

TEST_CASE_TEMPLATE("Parse a hexadecimal integer argument" * test_suite("scan"),
                   T, uint8_t, uint16_t, uint32_t, uint64_t) {
  argparse::ArgumentParser program("test");
  program.add_argument("-n").scan<'X', T>();

  SUBCASE("with hex digit") {
    program.parse_args({"test", "-n", "0x1a"});
    REQUIRE(program.get<T>("-n") == 0x1a);
  }

  SUBCASE("minus sign produces an optional argument") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "-0x1"}),
                      std::runtime_error);
  }

  SUBCASE("plus sign is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "+0x1a"}),
                      std::invalid_argument);
  }

  SUBCASE("does not fit") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "0XFFFFFFFFFFFFFFFF1"}),
                      std::range_error);
  }
}

TEST_CASE_TEMPLATE("Parse integer argument of any format" * test_suite("scan"),
                   T, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t,
                   uint32_t, uint64_t) {
  argparse::ArgumentParser program("test");
  program.add_argument("-n").scan<'i', T>();

  SUBCASE("zero") {
    program.parse_args({"test", "-n", "0"});
    REQUIRE(program.get<T>("-n") == 0);
  }

  SUBCASE("octal") {
    program.parse_args({"test", "-n", "077"});
    REQUIRE(program.get<T>("-n") == 077);
  }

  SUBCASE("no negative octal") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "-0777"}),
                      std::runtime_error);
  }

  SUBCASE("hex") {
    program.parse_args({"test", "-n", "0X2c"});
    REQUIRE(program.get<T>("-n") == 0X2c);
  }

  SUBCASE("no negative hex") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "-0X2A"}),
                      std::runtime_error);
  }

  SUBCASE("decimal") {
    program.parse_args({"test", "-n", "98"});
    REQUIRE(program.get<T>("-n") == 98);
  }

  SUBCASE("negative decimal") {
    if constexpr (std::is_signed_v<T>) {
      program.parse_args({"test", "-n", "-39"});
      REQUIRE(program.get<T>("-n") == -39);
    } else {
      REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "-39"}),
                        std::invalid_argument);
    }
  }

  SUBCASE("left-padding is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "\t32"}),
                      std::invalid_argument);
  }

  SUBCASE("right-padding is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "32\n"}),
                      std::invalid_argument);
  }

  SUBCASE("plus sign is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "+670"}),
                      std::invalid_argument);
  }
}

#define FLOAT_G(t, literal)                                                    \
  ([] {                                                                        \
    if constexpr (std::is_same_v<t, float>)                                    \
      return literal##f;                                                       \
    else if constexpr (std::is_same_v<t, double>)                              \
      return literal;                                                          \
    else if constexpr (std::is_same_v<t, long double>)                         \
      return literal##l;                                                       \
  }())

TEST_CASE_TEMPLATE("Parse floating-point argument of general format" *
                       test_suite("scan"),
                   T, float, double, long double) {
  argparse::ArgumentParser program("test");
  program.add_argument("-n").scan<'g', T>();

  SUBCASE("zero") {
    program.parse_args({"test", "-n", "0"});
    REQUIRE(program.get<T>("-n") == 0.);
  }

  SUBCASE("non-negative") {
    program.parse_args({"test", "-n", "3.14"});
    REQUIRE(program.get<T>("-n") == FLOAT_G(T, 3.14));
  }

  SUBCASE("negative") {
    program.parse_args({"test", "-n", "-0.12"});
    REQUIRE(program.get<T>("-n") == FLOAT_G(T, -0.12));
  }

  SUBCASE("left-padding is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "\t.32"}),
                      std::invalid_argument);
  }

  SUBCASE("right-padding is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", ".32\n"}),
                      std::invalid_argument);
  }

  SUBCASE("plus sign is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "+.12"}),
                      std::invalid_argument);
  }

  SUBCASE("plus sign after padding is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "   +.12"}),
                      std::invalid_argument);
  }

  SUBCASE("hexfloat is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "0x1a.3p+1"}),
                      std::invalid_argument);
  }

  SUBCASE("does not fit") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "1.3e+5000"}),
                      std::range_error);
  }
}

TEST_CASE_TEMPLATE("Parse hexadecimal floating-point argument" *
                       test_suite("scan"),
                   T, float, double, long double) {
  argparse::ArgumentParser program("test");
  program.add_argument("-n").scan<'a', T>();

  SUBCASE("zero") {
    // binary-exponent-part is not optional in C++ grammar
    program.parse_args({"test", "-n", "0x0"});
    REQUIRE(program.get<T>("-n") == 0x0.p0);
  }

  SUBCASE("non-negative") {
    program.parse_args({"test", "-n", "0x1a.3p+1"});
    REQUIRE(program.get<T>("-n") == 0x1a.3p+1);
  }

  SUBCASE("minus sign produces an optional argument") {
    // XXX may worth a fix
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "-0x0.12p1"}),
                      std::runtime_error);
  }

  SUBCASE("plus sign is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "+0x1p0"}),
                      std::invalid_argument);
  }

  SUBCASE("general format is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "3.14"}),
                      std::invalid_argument);
  }
}

TEST_CASE_TEMPLATE("Parse floating-point argument of scientific format" *
                       test_suite("scan"),
                   T, float, double, long double) {
  argparse::ArgumentParser program("test");
  program.add_argument("-n").scan<'e', T>();

  SUBCASE("zero") {
    program.parse_args({"test", "-n", "0e0"});
    REQUIRE(program.get<T>("-n") == 0e0);
  }

  SUBCASE("non-negative") {
    program.parse_args({"test", "-n", "3.14e-1"});
    REQUIRE(program.get<T>("-n") == FLOAT_G(T, 3.14e-1));
  }

  SUBCASE("negative") {
    program.parse_args({"test", "-n", "-0.12e+1"});
    REQUIRE(program.get<T>("-n") == FLOAT_G(T, -0.12e+1));
  }

  SUBCASE("plus sign is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "+.12e+1"}),
                      std::invalid_argument);
  }

  SUBCASE("fixed format is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "3.14"}),
                      std::invalid_argument);
  }

  SUBCASE("hexfloat is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "0x1.33p+0"}),
                      std::invalid_argument);
  }

  SUBCASE("does not fit") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "1.3e+5000"}),
                      std::range_error);
  }
}

TEST_CASE_TEMPLATE("Parse floating-point argument of fixed format" *
                       test_suite("scan"),
                   T, float, double, long double) {
  argparse::ArgumentParser program("test");
  program.add_argument("-n").scan<'f', T>();

  SUBCASE("zero") {
    program.parse_args({"test", "-n", ".0"});
    REQUIRE(program.get<T>("-n") == .0);
  }

  SUBCASE("non-negative") {
    program.parse_args({"test", "-n", "3.14"});
    REQUIRE(program.get<T>("-n") == FLOAT_G(T, 3.14));
  }

  SUBCASE("negative") {
    program.parse_args({"test", "-n", "-0.12"});
    REQUIRE(program.get<T>("-n") == FLOAT_G(T, -0.12));
  }

  SUBCASE("plus sign is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "+.12"}),
                      std::invalid_argument);
  }

  SUBCASE("scientific format is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "3.14e+0"}),
                      std::invalid_argument);
  }

  SUBCASE("hexfloat is not allowed") {
    REQUIRE_THROWS_AS(program.parse_args({"test", "-n", "0x1.33p+0"}),
                      std::invalid_argument);
  }
}
