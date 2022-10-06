#include <argparse/argparse.hpp>
#include <doctest.hpp>
#include <test_utility.hpp>

using doctest::test_suite;

TEST_CASE("Parse vector of arguments" * test_suite("vector")) {
  argparse::ArgumentParser program("test");
  program.add_argument("input").nargs(2);

  program.parse_args({"test", "rocket.mesh", "thrust_profile.csv"});

  auto inputs = program.get<std::vector<std::string>>("input");
  REQUIRE(inputs.size() == 2);
  REQUIRE(inputs[0] == "rocket.mesh");
  REQUIRE(inputs[1] == "thrust_profile.csv");
}

TEST_CASE("Parse list of arguments" * test_suite("vector")) {
  argparse::ArgumentParser program("test");
  program.add_argument("input").nargs(2);

  program.parse_args({"test", "rocket.mesh", "thrust_profile.csv"});

  auto inputs = program.get<std::list<std::string>>("input");
  REQUIRE(inputs.size() == 2);
  REQUIRE(testutility::get_from_list(inputs, 0) == "rocket.mesh");
  REQUIRE(testutility::get_from_list(inputs, 1) == "thrust_profile.csv");
}

TEST_CASE("Parse list of arguments with default values" *
          test_suite("vector")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--input")
      .default_value(std::list<int>{1, 2, 3, 4, 5})
      .nargs(5);

  program.parse_args({"test"});

  auto inputs = program.get<std::list<int>>("--input");
  REQUIRE(inputs.size() == 5);
  REQUIRE(testutility::get_from_list(inputs, 0) == 1);
  REQUIRE(testutility::get_from_list(inputs, 1) == 2);
  REQUIRE(testutility::get_from_list(inputs, 2) == 3);
  REQUIRE(testutility::get_from_list(inputs, 3) == 4);
  REQUIRE(testutility::get_from_list(inputs, 4) == 5);
  REQUIRE(program["--input"] == std::list<int>{1, 2, 3, 4, 5});
}

TEST_CASE("Parse list of arguments and save in an object" *
          test_suite("vector")) {

  struct ConfigManager {
    std::vector<std::string> files;
    void add_file(const std::string &file) { files.push_back(file); }
  };

  ConfigManager config_manager;

  argparse::ArgumentParser program("test");
  program.add_argument("--input_files")
      .nargs(2)
      .action([&](const std::string &value) {
        config_manager.add_file(value);
        return value;
      });

  program.parse_args({"test", "--input_files", "config.xml", "system.json"});

  auto file_args = program.get<std::vector<std::string>>("--input_files");
  REQUIRE(file_args.size() == 2);
  REQUIRE(file_args[0] == "config.xml");
  REQUIRE(file_args[1] == "system.json");

  REQUIRE(config_manager.files.size() == 2);
  REQUIRE(config_manager.files[0] == "config.xml");
  REQUIRE(config_manager.files[1] == "system.json");
  REQUIRE(program["--input_files"] ==
          std::vector<std::string>{"config.xml", "system.json"});
}
