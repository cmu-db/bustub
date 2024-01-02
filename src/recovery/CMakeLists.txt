add_library(
  bustub_recovery
  OBJECT
  log_manager.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_recovery>
  PARENT_SCOPE)
