add_library(
  bustub_execution
  OBJECT
  aggregation_executor.cpp
  delete_executor.cpp
  distinct_executor.cpp
  fmt_impl.cpp
  hash_join_executor.cpp
  index_scan_executor.cpp
  insert_executor.cpp
  limit_executor.cpp
  nested_index_join_executor.cpp
  nested_loop_join_executor.cpp
  seq_scan_executor.cpp
  update_executor.cpp
  executor_factory.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_execution>
    PARENT_SCOPE)
