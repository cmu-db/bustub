#include "binder/expressions/bound_agg_call.h"
#include "fmt/format.h"

namespace bustub {
auto BoundAggCall::ToString() const -> std::string { return fmt::format("{}({})", func_name_, fmt::join(args_, ", ")); }
}  // namespace bustub
