add_library(
  bustub_catalog
  OBJECT
  column.cpp
  table_generator.cpp
  schema.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_catalog>
    PARENT_SCOPE)
