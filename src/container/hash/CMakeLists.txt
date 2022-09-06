add_library(
  bustub_container_hash
  OBJECT
        extendible_hash_table.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_container_hash>
    PARENT_SCOPE)
