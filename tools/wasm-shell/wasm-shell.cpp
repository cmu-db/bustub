#include <cmath>

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include "binder/binder.h"
#include "common/bustub_instance.h"
#include "common/exception.h"
#include "common/util/string_util.h"
#include "fmt/core.h"
#include "linenoise/linenoise.h"
#include "utf8proc/utf8proc.h"

static std::unique_ptr<bustub::BustubInstance> instance = nullptr;

extern "C" {

auto BustubInit() -> int {
  std::cout << "Initialize BusTub..." << std::endl;
  auto bustub = std::make_unique<bustub::BustubInstance>();
  bustub->GenerateMockTable();

  if (bustub->buffer_pool_manager_ != nullptr) {
    bustub->GenerateTestTable();
  }

  bustub->EnableManagedTxn();

  instance = std::move(bustub);
  return 0;
}

auto BustubExecuteQuery(const char *input, char *prompt, char *output, uint16_t len) -> int {
  std::string input_string(input);
  std::cout << input_string << std::endl;
  std::string output_string;
  std::string output_prompt;
  try {
    auto writer = bustub::HtmlWriter();
    instance->ExecuteSql(input_string, writer);
    output_string = writer.ss_.str();
  } catch (bustub::Exception &ex) {
    output_string = ex.what();
  }
  auto txn = instance->CurrentManagedTxn();
  if (txn != nullptr) {
    output_prompt = fmt::format("txn{}", txn->GetTransactionIdHumanReadable());
  }
  strncpy(output, output_string.c_str(), len);
  strncpy(prompt, output_prompt.c_str(), len);
  if (output_string.length() >= len) {
    return 1;
  }
  return 0;
}
}
