add_library(
  bustub_primer
  OBJECT
  hyperloglog.cpp
  hyperloglog_presto.cpp
  orset.cpp
  orset_driver.cpp
  skiplist.cpp
  trie.cpp
  trie_store.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_primer>
  PARENT_SCOPE)
