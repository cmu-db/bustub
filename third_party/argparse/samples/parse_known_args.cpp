#include <argparse/argparse.hpp>
#include <cassert>

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("test");
  program.add_argument("--foo").implicit_value(true).default_value(false);
  program.add_argument("bar");

  auto unknown_args = program.parse_known_args(argc, argv);

  if (program.is_used("--foo")) {
    std::cout << "--foo        : " << program.get<bool>("--foo") << "\n";
  }

  if (program.is_used("bar")) {
    std::cout << "bar          : " << program.get<std::string>("bar") << "\n";
  }

  if (!unknown_args.empty()) {
    std::cout << "Unknown args : ";
    for (const auto &u : unknown_args) {
      std::cout << u << " ";
    }
    std::cout << std::endl;
  }
}