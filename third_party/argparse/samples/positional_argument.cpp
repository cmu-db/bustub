#include <argparse/argparse.hpp>

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("main");

  program.add_argument("square")
      .help("display the square of a given number")
      .scan<'i', int>();

  program.add_argument("--verbose").default_value(false).implicit_value(true);

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  int input = program.get<int>("square");

  if (program["--verbose"] == true) {
    std::cout << "The square of " << input << " is " << (input * input)
              << std::endl;
  } else {
    std::cout << (input * input) << std::endl;
  }
}
