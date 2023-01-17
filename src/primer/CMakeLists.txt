add_library(
  bustub_primer
  OBJECT
  trie.cpp
  trie_store.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_primer>
  PARENT_SCOPE)
