add_library(
  bustub_primer
  OBJECT
  p0_trie.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_primer>
    PARENT_SCOPE)
