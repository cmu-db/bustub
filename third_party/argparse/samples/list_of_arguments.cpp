#include <argparse/argparse.hpp>

int main(int argc, char *argv[]) {

  argparse::ArgumentParser program("main");

  program.add_argument("--input_files")
      .help("The list of input files")
      .nargs(2);

  try {
    program.parse_args(
        argc, argv); // Example: ./main --input_files config.yml System.xml
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  auto files = program.get<std::vector<std::string>>(
      "--input_files"); // {"config.yml", "System.xml"}

  if (!files.empty()) {
    std::cout << "Files: ";
    for (auto &file : files) {
      std::cout << file << " ";
    }
    std::cout << std::endl;
  }
}