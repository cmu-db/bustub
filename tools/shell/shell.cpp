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
  linenoiseSetMultiLine(1);

  auto prompt = use_emoji_prompt ? emoji_prompt : default_prompt;

  while (true) {
    std::string query;
    bool first_line = true;
    while (true) {
      auto line_prompt = first_line ? prompt : "... ";
      if (!disable_tty) {
        char *query_c_str = linenoise(line_prompt);
        if (query_c_str == nullptr) {
          return 0;
        }
        query += query_c_str;
        if (bustub::StringUtil::EndsWith(query, ";") || bustub::StringUtil::StartsWith(query, "\\")) {
          break;
        }
        query += " ";
        linenoiseFree(query_c_str);
      } else {
        std::string query_line;
        std::cout << line_prompt;
        std::getline(std::cin, query_line);
        if (!std::cin) {
          return 0;
        }
        query += query_line;
        if (bustub::StringUtil::EndsWith(query, ";") || bustub::StringUtil::StartsWith(query, "\\")) {
          break;
        }
        query += "\n";
      }
      first_line = false;
    }

    if (!disable_tty) {
      linenoiseHistoryAdd(query.c_str());
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

  return 0;
}
