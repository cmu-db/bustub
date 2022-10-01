#include <argparse/argparse.hpp>
#include <cassert>

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("test");
  program.set_prefix_chars("-+/");
  program.set_assign_chars("=:");

  program.add_argument("--foo");
  program.add_argument("/B");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  if (program.is_used("--foo")) {
    std::cout << "--foo : " << program.get("--foo") << "\n";
  }

  if (program.is_used("/B")) {
    std::cout << "/B    : " << program.get("/B") << "\n";
  }
}