add_library(
    bustub_optimizer
    OBJECT
    merge_projection.cpp
    optimizer.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_optimizer>
    PARENT_SCOPE)
