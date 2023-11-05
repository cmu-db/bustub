#include "txn_common.h"  // NOLINT

namespace bustub {

TEST(TxnTsTest, DISABLED_WatermarkPerformance) {  // NOLINT
  const int txn_n = 1000000;
  {
    auto watermark = Watermark(0);
    for (int i = 0; i < txn_n; i++) {
      watermark.AddTxn(i);
      ASSERT_EQ(watermark.GetWatermark(), 0);
    }
    for (int i = 0; i < txn_n; i++) {
      watermark.UpdateCommitTs(i + 1);
      watermark.RemoveTxn(i);
      ASSERT_EQ(watermark.GetWatermark(), i + 1);
    }
  }
  {
    auto watermark = Watermark(0);
    for (int i = 0; i < txn_n; i++) {
      watermark.AddTxn(i);
      ASSERT_EQ(watermark.GetWatermark(), 0);
    }
    for (int i = 0; i < txn_n; i++) {
      watermark.UpdateCommitTs(i + 1);
      watermark.RemoveTxn(txn_n - i - 1);
      if (i == txn_n - 1) {
        ASSERT_EQ(watermark.GetWatermark(), txn_n);
      } else {
        ASSERT_EQ(watermark.GetWatermark(), 0);
      }
    }
  }
}

TEST(TxnTsTest, DISABLED_TimestampTracking) {  // NOLINT
  auto bustub = std::make_unique<BustubInstance>();
  auto txn0 = bustub->txn_manager_->Begin();
  ASSERT_EQ(txn0->GetReadTs(), 0);
  ASSERT_EQ(bustub->txn_manager_->GetWatermark(), 0);
  bustub->txn_manager_->Commit(txn0);
  ASSERT_EQ(bustub->txn_manager_->GetWatermark(), 1);

  auto txn1 = bustub->txn_manager_->Begin();
  ASSERT_EQ(txn1->GetReadTs(), 1);
  ASSERT_EQ(bustub->txn_manager_->GetWatermark(), 1);
  bustub->txn_manager_->Abort(txn1);
  ASSERT_EQ(bustub->txn_manager_->GetWatermark(), 1);

  // test case continues on Gradescope...
}

// NOLINTEND(bugprone-unchecked-optional-access))

}  // namespace bustub
