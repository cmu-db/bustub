#include <argparse/argparse.hpp>

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("compiler");

  program.add_argument("files").remaining();

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  try {
    auto files = program.get<std::vector<std::string>>("files");
    std::cout << files.size() << " files provided" << std::endl;
    for (auto &file : files)
      std::cout << file << std::endl;
  } catch (std::logic_error &e) {
    std::cout << "No files provided" << std::endl;
  }
}