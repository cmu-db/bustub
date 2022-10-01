#include <argparse/argparse.hpp>
#include <cassert>

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("test");
  program.set_prefix_chars("-+/");

  program.add_argument("+f");
  program.add_argument("--bar");
  program.add_argument("/foo");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  if (program.is_used("+f")) {
    std::cout << "+f    : " << program.get("+f") << "\n";
  }

  if (program.is_used("--bar")) {
    std::cout << "--bar : " << program.get("--bar") << "\n";
  }

  if (program.is_used("/foo")) {
    std::cout << "/foo  : " << program.get("/foo") << "\n";
  }
}