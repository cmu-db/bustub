add_library(
    bustub_optimizer
    OBJECT
    merge_projection.cpp
    merge_filter_nlj.cpp
    nlj_as_hash_join.cpp
    nlj_as_index_join.cpp
    optimizer.cpp
    order_by_index_scan.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_optimizer>
    PARENT_SCOPE)
