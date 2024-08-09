add_library(
        bustub_optimizer
        OBJECT
        eliminate_true_filter.cpp
        merge_projection.cpp
        merge_filter_nlj.cpp
        merge_filter_scan.cpp
        nlj_as_hash_join.cpp
        nlj_as_index_join.cpp
        optimizer.cpp
        optimizer_custom_rules.cpp
        optimizer_internal.cpp
        order_by_index_scan.cpp
        sort_limit_as_topn.cpp
        seqscan_as_indexscan.cpp
        column_pruning.cpp)

set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_optimizer>
        PARENT_SCOPE)
