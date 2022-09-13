add_library(
    bustub_optimizer
    OBJECT
    optimizer.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_optimizer>
    PARENT_SCOPE)
