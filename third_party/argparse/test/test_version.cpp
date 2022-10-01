#include <argparse/argparse.hpp>
#include <doctest.hpp>
#include <sstream>

using doctest::test_suite;

TEST_CASE("Users can print version and exit" * test_suite("version") *
          doctest::skip()) {
  argparse::ArgumentParser program("cli-test", "1.9.0");
  program.add_argument("-d", "--dir").required();
  program.parse_args({"test", "--version"});
  REQUIRE(program.get("--version") == "1.9.0");
}

TEST_CASE("Users can disable default -v/--version" * test_suite("version")) {
  argparse::ArgumentParser program("test", "1.0",
                                   argparse::default_arguments::help);
  REQUIRE_THROWS_WITH_AS(program.parse_args({"test", "--version"}),
                         "Unknown argument: --version", std::runtime_error);
}

TEST_CASE("Users can replace default -v/--version" * test_suite("version")) {
  std::string version{"3.1415"};
  argparse::ArgumentParser program("test", version,
                                   argparse::default_arguments::help);
  std::stringstream buffer;
  program.add_argument("-v", "--version")
      .action([&](const auto &) { buffer << version; })
      .default_value(true)
      .implicit_value(false)
      .nargs(0);

  REQUIRE(buffer.str().empty());
  program.parse_args({"test", "--version"});
  REQUIRE_FALSE(buffer.str().empty());
}
