add_library(
        bustub_buffer
        OBJECT
        buffer_pool_manager.cpp
        clock_replacer.cpp
        lru_replacer.cpp
        lru_k_replacer.cpp)

set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_buffer>
        PARENT_SCOPE)
