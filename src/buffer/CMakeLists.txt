add_library(
  bustub_buffer 
  OBJECT
  buffer_pool_manager_instance.cpp
  clock_replacer.cpp
  lru_replacer.cpp
  parallel_buffer_pool_manager.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_buffer>
    PARENT_SCOPE)
