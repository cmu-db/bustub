add_library(
  bustub_common
  OBJECT
  util/string_util.cpp
  config.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_common>
    PARENT_SCOPE)
