#include <argparse/argparse.hpp>

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("git");

  // git add subparser
  argparse::ArgumentParser add_command("add");
  add_command.add_description("Add file contents to the index");
  add_command.add_argument("files")
      .help("Files to add content from. Fileglobs (e.g.  *.c) can be given to "
            "add all matching files.")
      .remaining();

  // git commit subparser
  argparse::ArgumentParser commit_command("commit");
  commit_command.add_description("Record changes to the repository");
  commit_command.add_argument("-a", "--all")
      .help("Tell the command to automatically stage files that have been "
            "modified and deleted.")
      .default_value(false)
      .implicit_value(true);

  commit_command.add_argument("-m", "--message")
      .help("Use the given <msg> as the commit message.");

  // git cat-file subparser
  argparse::ArgumentParser catfile_command("cat-file");
  catfile_command.add_description(
      "Provide content or type and size information for repository objects");
  catfile_command.add_argument("-t").help(
      "Instead of the content, show the object type identified by <object>.");

  catfile_command.add_argument("-p").help(
      "Pretty-print the contents of <object> based on its type.");

  // git submodule subparser
  argparse::ArgumentParser submodule_command("submodule");
  submodule_command.add_description("Initialize, update or inspect submodules");
  argparse::ArgumentParser submodule_update_command("update");
  submodule_update_command.add_description(
      "Update the registered submodules to match what the superproject "
      "expects");
  submodule_update_command.add_argument("--init")
      .default_value(false)
      .implicit_value(true);
  submodule_update_command.add_argument("--recursive")
      .default_value(false)
      .implicit_value(true);
  submodule_command.add_subparser(submodule_update_command);

  program.add_subparser(add_command);
  program.add_subparser(commit_command);
  program.add_subparser(catfile_command);
  program.add_subparser(submodule_command);

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  // Use arguments
}
