#include <iostream>
#include <string>
#include "binder/binder.h"
#include "common/bustub_instance.h"
#include "common/exception.h"
#include "common/util/string_util.h"
#include "linenoise/linenoise.h"

// NOLINTNEXTLINE
auto main(int argc, char **argv) -> int {
  auto bustub = std::make_unique<bustub::BustubInstance>("test.db");

  auto default_prompt = "bustub> ";
  auto emoji_prompt = "\U0001f6c1> ";  // the bathtub emoji
  bool use_emoji_prompt = false;
  bool disable_tty = false;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--emoji-prompt") == 0) {
      use_emoji_prompt = true;
      break;
    }
    if (strcmp(argv[i], "--disable-tty") == 0) {
      disable_tty = true;
      break;
    }
  }

  bustub->GenerateMockTable();

  if (bustub->buffer_pool_manager_ != nullptr) {
    bustub->GenerateTestTable();
  }

  std::cout << "Welcome to the BusTub shell! Type \\help to learn more." << std::endl << std::endl;

  linenoiseHistorySetMaxLen(1024);

  if (disable_tty) {
    std::string query;
    while (true) {
      std::getline(std::cin, query);
      if (!std::cin) {
        break;
      }
      try {
        auto result = bustub->ExecuteSql(query);
        for (const auto &line : result) {
          std::cout << line << std::endl;
        }
      } catch (bustub::Exception &ex) {
        std::cerr << ex.what() << std::endl;
      }
    }
  } else {
    char *query_c_str;
    while ((query_c_str = linenoise(use_emoji_prompt ? emoji_prompt : default_prompt)) != nullptr) {
      std::string query(query_c_str);
      linenoiseHistoryAdd(query_c_str);
      linenoiseFree(query_c_str);
      try {
        auto result = bustub->ExecuteSql(query);
        for (const auto &line : result) {
          std::cout << line << std::endl;
        }
      } catch (bustub::Exception &ex) {
        std::cerr << ex.what() << std::endl;
      }
    }
  }

  return 0;
}
