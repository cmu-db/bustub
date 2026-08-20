set(PRIMER_SOURCES
  hyperloglog.cpp
  hyperloglog_presto.cpp
  orset.cpp
  orset_driver.cpp
  skiplist.cpp
  count_min_sketch.cpp
  trie.cpp
  trie_store.cpp)

# Robin Hood hashing is a private Project 0 exercise. The autograder copies a
# student's implementation before configuring Project 0, while other projects
# build from the public starter repository where this source file is absent.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/robin_hood_hashing.cpp")
  list(APPEND PRIMER_SOURCES robin_hood_hashing.cpp)
endif()

add_library(bustub_primer OBJECT ${PRIMER_SOURCES})

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_primer>
  PARENT_SCOPE)
