add_library(
    bustub_storage_table
    OBJECT
    table_heap.cpp
    table_iterator.cpp
    tuple.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_storage_table>
    PARENT_SCOPE)
