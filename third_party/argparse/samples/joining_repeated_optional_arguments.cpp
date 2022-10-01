#include <argparse/argparse.hpp>

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("test");

  program.add_argument("--color")
      .default_value<std::vector<std::string>>({"orange"})
      .append()
      .help("specify the cat's fur color");

  try {
    program.parse_args(
        argc, argv); // Example: ./main --color red --color green --color blue
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  auto colors = program.get<std::vector<std::string>>(
      "--color"); // {"red", "green", "blue"}

  std::cout << "Colors: ";
  for (const auto &c : colors) {
    std::cout << c << " ";
  }
  std::cout << "\n";
}
