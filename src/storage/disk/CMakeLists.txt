add_library(
    bustub_storage_disk 
    OBJECT
    disk_manager.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_storage_disk>
    PARENT_SCOPE)
