#include <argparse/argparse.hpp>

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("test");

  int verbosity = 0;
  program.add_argument("-V", "--verbose")
      .action([&](const auto &) { ++verbosity; })
      .append()
      .default_value(false)
      .implicit_value(true)
      .nargs(0);

  program.parse_args(argc, argv); // Example: ./main -VVVV

  std::cout << "verbose level: " << verbosity << std::endl; // verbose level: 4
}
