add_library(
  bustub_common
  OBJECT
  bustub_instance.cpp
  config.cpp
  util/string_util.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_common>
    PARENT_SCOPE)
