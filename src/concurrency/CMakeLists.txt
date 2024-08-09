add_library(
  bustub_concurrency
  OBJECT
  transaction_manager.cpp
  transaction_manager_impl.cpp
  watermark.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_concurrency>
  PARENT_SCOPE)
