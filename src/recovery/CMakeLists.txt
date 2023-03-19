add_library(
  bustub_recovery
  OBJECT
  checkpoint_manager.cpp
  log_manager.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_recovery>
  PARENT_SCOPE)
