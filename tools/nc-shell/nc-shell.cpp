//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// nc-shell.cpp
//
// Identification: tools/nc-shell/nc-shell.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <netinet/in.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <string>
#include <thread>
#include "argparse/argparse.hpp"
#include "binder/binder.h"
#include "common/bustub_instance.h"
#include "common/exception.h"
#include "common/util/string_util.h"
#include "concurrency/transaction.h"
#include "concurrency/transaction_manager.h"
#include "fmt/core.h"
#include "libfort/lib/fort.hpp"
#include "utf8proc/utf8proc.h"

auto GetWidthOfUtf8(const void *beg, const void *end, size_t *width) -> int {
  size_t computed_width = 0;
  utf8proc_ssize_t n;
  utf8proc_ssize_t size = static_cast<const char *>(end) - static_cast<const char *>(beg);
  auto pstring = static_cast<utf8proc_uint8_t const *>(beg);
  utf8proc_int32_t data;
  while ((n = utf8proc_iterate(pstring, size, &data)) > 0) {
    computed_width += utf8proc_charwidth(data);
    pstring += n;
    size -= n;
  }
  *width = computed_width;
  return 0;
}

void Serve(int socket, bustub::BusTubInstance *bustub, bool use_serializable_iso_lvl) {
  const std::string prompt = "bustub> ";
  char buffer[1024] = {0};

  std::string welcome = "Welcome to the BusTub shell! Type \\help to learn more.\n";

  auto txn = bustub->txn_manager_->Begin(use_serializable_iso_lvl ? bustub::IsolationLevel::SERIALIZABLE
                                                                  : bustub::IsolationLevel::SNAPSHOT_ISOLATION);

  fmt::println(stderr, "txn{}: started", txn->GetTransactionIdHumanReadable());

  send(socket, welcome.c_str(), welcome.length(), 0);

  while (true) {
    std::string txn_status =
        fmt::format("txn_id={} status={} read_ts={} iso_lvl={}\n", txn->GetTransactionIdHumanReadable(),
                    txn->GetTransactionState(), txn->GetReadTs(), txn->GetIsolationLevel());
    send(socket, txn_status.c_str(), txn_status.length(), 0);

    if (txn->GetTransactionState() != bustub::TransactionState::RUNNING) {
      close(socket);
      return;
    }

    std::string query;
    bool first_line = true;
    while (true) {
      auto line_prompt = first_line ? prompt : "... ";
      send(socket, line_prompt.c_str(), line_prompt.length(), 0);
      int valread = read(socket, buffer, 1024 - 1);
      if (valread <= 0) {
        fmt::println(stderr, "txn{}: disconnected, status={}", txn->GetTransactionIdHumanReadable(),
                     txn->GetTransactionState());
        if (txn->GetTransactionState() == bustub::TransactionState::RUNNING ||
            txn->GetTransactionState() == bustub::TransactionState::TAINTED) {
          bustub->txn_manager_->Abort(txn);
          fmt::println(stderr, "txn{}: aborted due to disconnected", txn->GetTransactionIdHumanReadable());
        }
        close(socket);
        return;
      }
      buffer[valread] = '\0';
      query += buffer;
      size_t last_len;
      do {
        last_len = query.length();
        query = bustub::StringUtil::Strip(query, '\n');
        query = bustub::StringUtil::Strip(query, '\r');
      } while (query.length() != last_len);

      if (bustub::StringUtil::EndsWith(query, ";") || bustub::StringUtil::StartsWith(query, "\\")) {
        break;
      }
      query += "\n";
      first_line = false;
    }
    std::cout << "txn" << txn->GetTransactionIdHumanReadable() << ": " << query << std::endl << std::flush;
    try {
      auto writer = bustub::FortTableWriter();
      if (!bustub->ExecuteSqlTxn(query, writer, txn)) {
        std::string res = "failed to execute\n";
        send(socket, res.c_str(), res.length(), 0);
        continue;
      }
      for (const auto &table : writer.tables_) {
        auto tt = table + "\n";
        send(socket, tt.c_str(), tt.length(), 0);
      }
    } catch (bustub::Exception &ex) {
      std::string what = ex.what();
      std::string what1 = "Error: " + what + "\n";
      send(socket, what1.c_str(), what1.length(), 0);
      std::cerr << ex.what() << std::endl;
    }
  }
}

void Cli(bustub::BusTubInstance *bustub) {
  const std::string prompt = "bustub> ";

  std::cout << "Welcome to the BusTub shell! Type \\help to learn more.\n";
  std::cout << "In this terminal, all statements are running in separate transactions. To create transactions, use "
               "the `nc` command to connect to BusTub.\n"
            << std::flush;

  while (true) {
    std::string query;
    bool first_line = true;
    while (true) {
      auto line_prompt = first_line ? prompt : "... ";
      std::cout << line_prompt;
      std::string query_line;
      std::getline(std::cin, query_line);
      query += query_line;
      if (bustub::StringUtil::EndsWith(query, ";") || bustub::StringUtil::StartsWith(query, "\\")) {
        break;
      }
      query += "\n";
      first_line = false;
    }
    try {
      auto writer = bustub::FortTableWriter();
      if (!bustub->ExecuteSql(query, writer)) {
        std::cout << "failed to execute" << std::endl << std::flush;
        continue;
      }
      for (const auto &table : writer.tables_) {
        std::cout << table << std::endl << std::flush;
      }
    } catch (bustub::Exception &ex) {
      std::cerr << ex.what() << std::endl;
    }
  }
}

// NOLINTNEXTLINE
auto main(int argc, char **argv) -> int {
  signal(SIGPIPE, SIG_IGN);

  ft_set_u8strwid_func(&GetWidthOfUtf8);

  auto bustub = std::make_unique<bustub::BusTubInstance>("db.bustub");

  argparse::ArgumentParser program("bustub-nc-shell");
  program.add_argument("--port").help("the port of the server");
  program.add_argument("--serializable")
      .help("use serializable isolation level")
      .default_value(false)
      .implicit_value(true);

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  int port = 23333;
  if (program.present("--port")) {
    port = std::stoi(program.get<std::string>("--port"));
  }

  bool use_serializable_iso_lvl = program.get<bool>("--serializable");

  if (use_serializable_iso_lvl) {
    std::cerr << "iso_lvl=SERIALIZABLE\n";
  } else {
    std::cerr << "iso_lvl=SNAPSHOT_ISOLATION\n";
  }

  bustub->GenerateMockTable();

  if (bustub->buffer_pool_manager_ != nullptr) {
    bustub->GenerateTestTable();
  }

  struct sockaddr_in address;
  int opt = 1;
  socklen_t addrlen = sizeof(address);

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  address.sin_port = htons(port);
  if (bind(server_fd, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  std::cerr << "Listening at " << port << std::endl;
  std::cerr << "connect with `nc 127.0.0.1 " << port << "`" << std::endl;

  std::thread cli(Cli, bustub.get());

  while (true) {
    int new_socket = accept(server_fd, reinterpret_cast<struct sockaddr *>(&address), &addrlen);
    if (new_socket < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }
    std::thread thread(Serve, new_socket, bustub.get(), use_serializable_iso_lvl);
    thread.detach();
  }

  return 0;
}
