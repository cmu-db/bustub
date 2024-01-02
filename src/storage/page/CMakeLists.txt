add_library(
    bustub_storage_page
    OBJECT
    b_plus_tree_internal_page.cpp
    b_plus_tree_leaf_page.cpp
    b_plus_tree_page.cpp
    hash_table_block_page.cpp
    hash_table_bucket_page.cpp
    hash_table_directory_page.cpp
    extendible_htable_bucket_page.cpp
    extendible_htable_directory_page.cpp
    extendible_htable_header_page.cpp
    extendible_htable_page_utils.cpp
    page_guard.cpp
    table_page.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_storage_page>
    PARENT_SCOPE)
