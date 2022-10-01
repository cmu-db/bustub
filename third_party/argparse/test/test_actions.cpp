#include <argparse/argparse.hpp>
#include <doctest.hpp>

using doctest::test_suite;

TEST_CASE("Users can use default value inside actions" *
          test_suite("actions")) {
  argparse::ArgumentParser program("test");
  program.add_argument("input").default_value("bar").action(
      [=](const std::string &value) {
        static const std::vector<std::string> choices = {"foo", "bar", "baz"};
        if (std::find(choices.begin(), choices.end(), value) != choices.end()) {
          return value;
        }
        return std::string{"bar"};
      });

  program.parse_args({"test", "fez"});
  REQUIRE(program.get("input") == "bar");
}

TEST_CASE("Users can add actions that return nothing" * test_suite("actions")) {
  argparse::ArgumentParser program("test");
  bool pressed = false;
  auto &arg = program.add_argument("button").action(
      [&](const std::string &) mutable { pressed = true; });

  REQUIRE_FALSE(pressed);

  SUBCASE("action performed") {
    program.parse_args({"test", "ignored"});
    REQUIRE(pressed);
  }

  SUBCASE("action performed and nothing overrides the default value") {
    arg.default_value(42);

    program.parse_args({"test", "ignored"});
    REQUIRE(pressed);
    REQUIRE(program.get<int>("button") == 42);
  }
}

class Image {
public:
  int w = 0, h = 0;

  void resize(std::string_view geometry) {
    std::stringstream s;
    s << geometry;
    s >> w;
    s.ignore();
    s >> h;
  }

  static auto create(int w, int h, std::string_view format) -> Image {
    auto factor = [=] {
      if (format == "720p")
        return std::min(1280. / w, 720. / h);
      else if (format == "1080p")
        return std::min(1920. / w, 1080. / h);
      else
        return 1.;
    }();

    return {static_cast<int>(w * factor), static_cast<int>(h * factor)};
  }
};

TEST_CASE("Users can bind arguments to actions" * test_suite("actions")) {
  argparse::ArgumentParser program("test");

  GIVEN("an default initialized object bounded by reference") {
    Image img;
    program.add_argument("--size").action(&Image::resize, std::ref(img));

    WHEN("provided no command-line arguments") {
      program.parse_args({"test"});

      THEN("the object is not updated") {
        REQUIRE(img.w == 0);
        REQUIRE(img.h == 0);
      }
    }

    WHEN("provided command-line arguments") {
      program.parse_args({"test", "--size", "320x98"});

      THEN("the object is updated accordingly") {
        REQUIRE(img.w == 320);
        REQUIRE(img.h == 98);
      }
    }
  }

  GIVEN("a command-line option waiting for the last argument in its action") {
    program.add_argument("format").action(Image::create, 400, 300);

    WHEN("provided such an argument") {
      program.parse_args({"test", "720p"});

      THEN("the option object is created as if providing all arguments") {
        auto img = program.get<Image>("format");

        REQUIRE(img.w == 960);
        REQUIRE(img.h == 720);
      }
    }

    WHEN("provided a different argument") {
      program.parse_args({"test", "1080p"});

      THEN("a different option object is created") {
        auto img = program.get<Image>("format");

        REQUIRE(img.w == 1440);
        REQUIRE(img.h == 1080);
      }
    }
  }
}

TEST_CASE("Users can use actions on nargs=ANY arguments" *
          test_suite("actions")) {
  argparse::ArgumentParser program("sum");

  int result = 0;
  program.add_argument("all")
      .nargs(argparse::nargs_pattern::any)
      .action(
          [](int &sum, std::string const &value) { sum += std::stoi(value); },
          std::ref(result));

  program.parse_args({"sum", "42", "100", "-3", "-20"});
  REQUIRE(result == 119);
}

TEST_CASE("Users can use actions on remaining arguments" *
          test_suite("actions")) {
  argparse::ArgumentParser program("concat");

  std::string result = "";
  program.add_argument("all").remaining().action(
      [](std::string &sum, const std::string &value) { sum += value; },
      std::ref(result));

  program.parse_args({"concat", "a", "-b", "-c", "--d"});
  REQUIRE(result == "a-b-c--d");
}

TEST_CASE("Users can run actions on parameterless optional arguments" *
          test_suite("actions")) {
  argparse::ArgumentParser program("test");

  GIVEN("a flag argument with a counting action") {
    int count = 0;
    program.add_argument("-V", "--verbose")
        .action([&](const auto &) { ++count; })
        .append()
        .default_value(false)
        .implicit_value(true)
        .nargs(0);

    WHEN("the flag is repeated") {
      program.parse_args({"test", "-VVVV"});

      THEN("the count increments once per use") {
        REQUIRE(program.get<bool>("-V"));
        REQUIRE(count == 4);
      }
    }
  }
}
