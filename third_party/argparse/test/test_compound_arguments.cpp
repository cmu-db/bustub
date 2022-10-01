#include <argparse/argparse.hpp>
#include <doctest.hpp>
#include <test_utility.hpp>

using doctest::test_suite;

TEST_CASE("Parse compound toggle arguments with implicit values" *
          test_suite("compound_arguments")) {
  argparse::ArgumentParser program("test");
  program.add_argument("-a").default_value(false).implicit_value(true);

  program.add_argument("-u").default_value(false).implicit_value(true);

  program.add_argument("-x").default_value(false).implicit_value(true);

  program.parse_args({"./test.exe", "-aux"});
  REQUIRE(program.get<bool>("-a") == true);
  REQUIRE(program.get<bool>("-u") == true);
  REQUIRE(program.get<bool>("-x") == true);
}

TEST_CASE("Parse compound toggle arguments with implicit values and nargs" *
          test_suite("compound_arguments")) {
  argparse::ArgumentParser program("test");
  program.add_argument("-a").default_value(false).implicit_value(true);

  program.add_argument("-b").default_value(false).implicit_value(true);

  program.add_argument("-c").nargs(2).scan<'g', float>();

  program.add_argument("--input_files").nargs(3);

  program.parse_args({"./test.exe", "-abc", "3.14", "2.718", "--input_files",
                      "a.txt", "b.txt", "c.txt"});
  REQUIRE(program.get<bool>("-a") == true);
  REQUIRE(program.get<bool>("-b") == true);
  auto c = program.get<std::vector<float>>("-c");
  REQUIRE(c.size() == 2);
  REQUIRE(c[0] == 3.14f);
  REQUIRE(c[1] == 2.718f);
  auto input_files = program.get<std::vector<std::string>>("--input_files");
  REQUIRE(input_files.size() == 3);
  REQUIRE(input_files[0] == "a.txt");
  REQUIRE(input_files[1] == "b.txt");
  REQUIRE(input_files[2] == "c.txt");
}

TEST_CASE("Parse compound toggle arguments with implicit values and nargs and "
          "other positional arguments" *
          test_suite("compound_arguments")) {
  argparse::ArgumentParser program("test");

  program.add_argument("numbers").nargs(3).scan<'i', int>();

  program.add_argument("-a").default_value(false).implicit_value(true);

  program.add_argument("-b").default_value(false).implicit_value(true);

  program.add_argument("-c").nargs(2).scan<'g', float>();

  program.add_argument("--input_files").nargs(3);

  REQUIRE_THROWS(
      program.parse_args({"./test.exe", "1", "-abc", "3.14", "2.718", "2",
                          "--input_files", "a.txt", "b.txt", "c.txt", "3"}));
}

TEST_CASE("Parse out-of-order compound arguments" *
          test_suite("compound_arguments")) {
  argparse::ArgumentParser program("test");

  program.add_argument("-a").default_value(false).implicit_value(true);

  program.add_argument("-b").default_value(false).implicit_value(true);

  program.add_argument("-c").nargs(2).scan<'g', float>();

  program.parse_args({"./main", "-cab", "3.14", "2.718"});

  auto a = program.get<bool>("-a");               // true
  auto b = program.get<bool>("-b");               // true
  auto c = program.get<std::vector<float>>("-c"); // {3.14f, 2.718f}
  REQUIRE(a == true);
  REQUIRE(b == true);
  REQUIRE(program["-c"] == std::vector<float>{3.14f, 2.718f});
}

TEST_CASE("Parse out-of-order compound arguments. Second variation" *
          test_suite("compound_arguments")) {
  argparse::ArgumentParser program("test");

  program.add_argument("-a").default_value(false).implicit_value(true);

  program.add_argument("-b").default_value(false).implicit_value(true);

  program.add_argument("-c")
      .nargs(2)
      .default_value(std::vector<float>{0.0f, 0.0f})
      .scan<'g', float>();

  program.parse_args({"./main", "-cb"});

  auto a = program.get<bool>("-a");
  auto b = program.get<bool>("-b");
  auto c = program.get<std::vector<float>>("-c");

  REQUIRE(a == false);
  REQUIRE(b == true);
  REQUIRE(program["-c"] == std::vector<float>{0.0f, 0.0f});
}
