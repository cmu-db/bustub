#include <argparse/argparse.hpp>
#include <doctest.hpp>

using doctest::test_suite;

TEST_CASE("Missing argument" * test_suite("parse_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--config").nargs(1);
  REQUIRE_THROWS_WITH_AS(program.parse_args({"test", "--config"}),
                         "Too few arguments for '--config'.",
                         std::runtime_error);
}

TEST_CASE("Parse a string argument with value" * test_suite("parse_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--config");
  program.parse_args({"test", "--config", "config.yml"});
  REQUIRE(program.get("--config") == "config.yml");
}

TEST_CASE("Parse a string argument with default value" *
          test_suite("parse_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--config").default_value(std::string("foo.yml"));
  program.parse_args({"test", "--config"});
  REQUIRE(program.get("--config") == "foo.yml");
}

TEST_CASE("Parse a string argument without default value" *
          test_suite("parse_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--config");

  WHEN("no value provided") {
    program.parse_args({"test"});

    THEN("the option is nullopt") {
      auto opt = program.present("--config");
      REQUIRE_FALSE(opt);
      REQUIRE(opt == std::nullopt);
    }
  }

  WHEN("a value is provided") {
    program.parse_args({"test", "--config", ""});

    THEN("the option has a value") {
      auto opt = program.present("--config");
      REQUIRE(opt);
      REQUIRE(opt->empty());
    }
  }
}

TEST_CASE("Parse an int argument with value" * test_suite("parse_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--count").scan<'i', int>();
  program.parse_args({"test", "--count", "5"});
  REQUIRE(program.get<int>("--count") == 5);
}

TEST_CASE("Parse an int argument with default value" *
          test_suite("parse_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--count").default_value(2).scan<'i', int>();
  program.parse_args({"test", "--count"});
  REQUIRE(program.get<int>("--count") == 2);
}

TEST_CASE("Parse a float argument with value" * test_suite("parse_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--ratio").scan<'g', float>();
  program.parse_args({"test", "--ratio", "5.6645"});
  REQUIRE(program.get<float>("--ratio") == 5.6645f);
}

TEST_CASE("Parse a float argument with default value" *
          test_suite("parse_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--ratio").default_value(3.14f).scan<'g', float>();
  program.parse_args({"test", "--ratio"});
  REQUIRE(program.get<float>("--ratio") == 3.14f);
}

TEST_CASE("Parse a double argument with value" * test_suite("parse_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--ratio").scan<'g', double>();
  program.parse_args({"test", "--ratio", "5.6645"});
  REQUIRE(program.get<double>("--ratio") == 5.6645);
}

TEST_CASE("Parse a double argument with default value" *
          test_suite("parse_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--ratio").default_value(3.14).scan<'g', double>();
  program.parse_args({"test", "--ratio"});
  REQUIRE(program.get<double>("--ratio") == 3.14);
}

TEST_CASE("Parse a vector of integer arguments" * test_suite("parse_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--vector").nargs(5).scan<'i', int>();
  program.parse_args({"test", "--vector", "1", "2", "3", "4", "5"});
  auto vector = program.get<std::vector<int>>("--vector");
  REQUIRE(vector.size() == 5);
  REQUIRE(vector[0] == 1);
  REQUIRE(vector[1] == 2);
  REQUIRE(vector[2] == 3);
  REQUIRE(vector[3] == 4);
  REQUIRE(vector[4] == 5);
}

TEST_CASE("Parse a vector of float arguments" * test_suite("parse_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--vector").nargs(5).scan<'g', float>();
  program.parse_args({"test", "--vector", "1.1", "2.2", "3.3", "4.4", "5.5"});
  auto vector = program.get<std::vector<float>>("--vector");
  REQUIRE(vector.size() == 5);
  REQUIRE(vector[0] == 1.1f);
  REQUIRE(vector[1] == 2.2f);
  REQUIRE(vector[2] == 3.3f);
  REQUIRE(vector[3] == 4.4f);
  REQUIRE(vector[4] == 5.5f);
}

TEST_CASE("Parse a vector of float without default value" *
          test_suite("parse_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--vector").scan<'g', float>().nargs(3);

  WHEN("no value is provided") {
    program.parse_args({"test"});

    THEN("the option is nullopt") {
      auto opt = program.present<std::vector<float>>("--vector");
      REQUIRE_FALSE(opt.has_value());
      REQUIRE(opt == std::nullopt);
    }
  }

  WHEN("a value is provided") {
    program.parse_args({"test", "--vector", ".3", "1.3", "6"});

    THEN("the option has a value") {
      auto opt = program.present<std::vector<float>>("--vector");
      REQUIRE(opt.has_value());

      auto &&vec = opt.value();
      REQUIRE(vec.size() == 3);
      REQUIRE(vec[0] == .3f);
      REQUIRE(vec[1] == 1.3f);
      REQUIRE(vec[2] == 6.f);
    }
  }
}

TEST_CASE("Parse a vector of double arguments" * test_suite("parse_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--vector").nargs(5).scan<'g', double>();
  program.parse_args({"test", "--vector", "1.1", "2.2", "3.3", "4.4", "5.5"});
  auto vector = program.get<std::vector<double>>("--vector");
  REQUIRE(vector.size() == 5);
  REQUIRE(vector[0] == 1.1);
  REQUIRE(vector[1] == 2.2);
  REQUIRE(vector[2] == 3.3);
  REQUIRE(vector[3] == 4.4);
  REQUIRE(vector[4] == 5.5);
}

TEST_CASE("Parse a vector of string arguments" * test_suite("parse_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--vector").nargs(5);
  program.parse_args({"test", "--vector", "abc", "def", "ghi", "jkl", "mno"});
  auto vector = program.get<std::vector<std::string>>("--vector");
  REQUIRE(vector.size() == 5);
  REQUIRE(vector[0] == "abc");
  REQUIRE(vector[1] == "def");
  REQUIRE(vector[2] == "ghi");
  REQUIRE(vector[3] == "jkl");
  REQUIRE(vector[4] == "mno");
}

TEST_CASE("Parse a vector of character arguments" * test_suite("parse_args")) {
  argparse::ArgumentParser program("test");
  program.add_argument("--vector")
      .nargs(5)
      .action([](const std::string &value) { return value[0]; });
  program.parse_args({"test", "--vector", "a", "b", "c", "d", "e"});
  auto vector = program.get<std::vector<char>>("--vector");
  REQUIRE(vector.size() == 5);
  REQUIRE(vector[0] == 'a');
  REQUIRE(vector[1] == 'b');
  REQUIRE(vector[2] == 'c');
  REQUIRE(vector[3] == 'd');
  REQUIRE(vector[4] == 'e');
}

TEST_CASE("Parse a vector of string arguments and construct objects" *
          test_suite("parse_args")) {

  class Foo {
  public:
    Foo(const std::string &value) : m_value(value) {}
    std::string m_value;
  };

  argparse::ArgumentParser program("test");
  program.add_argument("--vector")
      .nargs(5)
      .action([](const std::string &value) { return Foo(value); });
  program.parse_args({"test", "--vector", "abc", "def", "ghi", "jkl", "mno"});
  auto vector = program.get<std::vector<Foo>>("--vector");
  REQUIRE(vector.size() == 5);
  REQUIRE(vector[0].m_value == Foo("abc").m_value);
  REQUIRE(vector[1].m_value == Foo("def").m_value);
  REQUIRE(vector[2].m_value == Foo("ghi").m_value);
  REQUIRE(vector[3].m_value == Foo("jkl").m_value);
  REQUIRE(vector[4].m_value == Foo("mno").m_value);
}
