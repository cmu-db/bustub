add_library(
    bustub_optimizer
    OBJECT
    merge_projection.cpp
    merge_filter_nlj.cpp
    nlj_as_hash_join.cpp
    optimizer.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_optimizer>
    PARENT_SCOPE)
