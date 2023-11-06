#include <chrono>  // NOLINT
#include <exception>
#include <mutex>   // NOLINT
#include <thread>  // NOLINT
#include "common/macros.h"
#include "execution/execution_common.h"
#include "fmt/core.h"
#include "txn_common.h"  // NOLINT

namespace bustub {

// NOLINTBEGIN(bugprone-unchecked-optional-access)

TEST(TxnIndexTest, DISABLED_IndexInsertTest) {  // NOLINT
  auto bustub = std::make_unique<BustubInstance>();
  const std::string query = "SELECT * FROM maintable";

  Execute(*bustub, "CREATE TABLE maintable(a int primary key, b int)");
  auto table_info = bustub->catalog_->GetTable("maintable");
  auto txn1 = BeginTxn(*bustub, "txn1");
  WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1, 0)"));
  TxnMgrDbg("after txn1 insert", bustub->txn_manager_.get(), table_info, table_info->table_.get());
  WithTxn(txn1, QueryShowResult(*bustub, _var, _txn, query,
                                IntResult{
                                    {1, 0},
                                }));
  WithTxn(txn1, ExecuteTxnTainted(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1, 1)"));
  TxnMgrDbg("after txn1 taint", bustub->txn_manager_.get(), table_info, table_info->table_.get());
  TableHeapEntryNoMoreThan(*bustub, table_info, 1);

  auto txn2 = BeginTxn(*bustub, "txn2");
  WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (2, 2)"));
  TxnMgrDbg("after txn2 insert", bustub->txn_manager_.get(), table_info, table_info->table_.get());
  WithTxn(txn2, QueryShowResult(*bustub, _var, _txn, query,
                                IntResult{
                                    {2, 2},
                                }));
  WithTxn(txn2, CommitTxn(*bustub, _var, _txn));
  TxnMgrDbg("after txn2 commit", bustub->txn_manager_.get(), table_info, table_info->table_.get());
  TableHeapEntryNoMoreThan(*bustub, table_info, 2);

  auto txn3 = BeginTxn(*bustub, "txn3");
  WithTxn(txn3, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (3, 3)"));
  TxnMgrDbg("after txn3 insert", bustub->txn_manager_.get(), table_info, table_info->table_.get());
  WithTxn(txn3, QueryShowResult(*bustub, _var, _txn, query,
                                IntResult{
                                    {2, 2},
                                    {3, 3},
                                }));
  WithTxn(txn3, CommitTxn(*bustub, _var, _txn));
  TxnMgrDbg("after txn3 commit", bustub->txn_manager_.get(), table_info, table_info->table_.get());
  TableHeapEntryNoMoreThan(*bustub, table_info, 3);

  auto txn4 = BeginTxn(*bustub, "txn4");
  WithTxn(txn4, QueryShowResult(*bustub, _var, _txn, query,
                                IntResult{
                                    {2, 2},
                                    {3, 3},
                                }));
  WithTxn(txn4, ExecuteTxnTainted(*bustub, _var, _txn, "INSERT INTO maintable VALUES (3, 4)"));
  TxnMgrDbg("after txn4 taint", bustub->txn_manager_.get(), table_info, table_info->table_.get());
  TableHeapEntryNoMoreThan(*bustub, table_info, 3);

  auto txn5 = BeginTxn(*bustub, "txn5");
  WithTxn(txn5, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (4, 4)"));
  TxnMgrDbg("after txn5 insert", bustub->txn_manager_.get(), table_info, table_info->table_.get());
  WithTxn(txn5, QueryShowResult(*bustub, _var, _txn, query,
                                IntResult{
                                    {2, 2},
                                    {3, 3},
                                    {4, 4},
                                }));
  TableHeapEntryNoMoreThan(*bustub, table_info, 4);

  auto txn6 = BeginTxn(*bustub, "txn6");
  WithTxn(txn6, QueryShowResult(*bustub, _var, _txn, query,
                                IntResult{
                                    {2, 2},
                                    {3, 3},
                                }));
}

TEST(TxnIndexTest, DISABLED_IndexConcurrentInsertTest) {  // NOLINT
  const auto generate_sql = [](int thread_id, int n) -> std::string {
    return fmt::format("INSERT INTO maintable VALUES ({}, {})", n, thread_id);
  };
  for (int n = 0; n < 50; n++) {
    auto bustub = std::make_unique<BustubInstance>();
    Execute(*bustub, "CREATE TABLE maintable(a int primary key, b int)");
    std::vector<std::thread> insert_threads;
    const int thread_cnt = 8;
    const int number_cnt = 80;
    insert_threads.reserve(thread_cnt);
    std::map<int, std::vector<bool>> operation_result;
    std::mutex result_mutex;
    fmt::println(stderr, "trial {}: running with {} threads with {} rows", n + 1, thread_cnt, number_cnt);
    global_disable_execution_exception_print.store(true);
    for (int thread = 0; thread < thread_cnt; thread++) {
      insert_threads.emplace_back([&bustub, thread, generate_sql, &result_mutex, &operation_result]() {
        NoopWriter writer;
        std::vector<bool> result;
        result.reserve(number_cnt);
        for (int i = 0; i < number_cnt; i++) {
          auto sql = generate_sql(thread, i);
          auto *txn = bustub->txn_manager_->Begin();
          if (bustub->ExecuteSqlTxn(sql, writer, txn)) {
            result.push_back(true);
            BUSTUB_ENSURE(bustub->txn_manager_->Commit(txn), "cannot commit??");
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
          } else {
            result.push_back(false);
          }
        }
        {
          std::lock_guard<std::mutex> lck(result_mutex);
          operation_result.emplace(thread, std::move(result));
        }
      });
    }
    for (auto &&thread : insert_threads) {
      thread.join();
    }
    global_disable_execution_exception_print.store(false);
    std::vector<std::vector<int>> expected_rows;
    std::map<int, int> winner_stats;
    for (int i = 0; i < number_cnt; i++) {
      int winner = -1;
      for (int j = 0; j < thread_cnt; j++) {
        if (operation_result[j][i]) {
          if (winner != -1) {
            fmt::println(stderr, "multiple winner for inserting {}: [{}]", i, fmt::join(operation_result[j], ","));
            std::terminate();
          }
          winner = j;
        }
      }
      if (winner == -1) {
        fmt::println(stderr, "no winner for inserting {}");
        std::terminate();
      }
      winner_stats[winner]++;
      expected_rows.push_back({i, winner});
    }
    for (auto &&[winner, cnt] : winner_stats) {
      if (cnt == number_cnt) {
        fmt::println(stderr, "WARNING: biased winner {}: cnt={}", winner, cnt);
        std::terminate();
      }
    }
    auto query_txn = BeginTxn(*bustub, "query_txn");
    WithTxn(query_txn, QueryShowResult(*bustub, _var, _txn, "SELECT * FROM maintable", expected_rows));
    auto entry = TableHeapEntry(*bustub, bustub->catalog_->GetTable("maintable"));
    fmt::println("{} entries in the table heap", entry);
  }
}

// NOLINTEND(bugprone-unchecked-optional-access))

}  // namespace bustub
