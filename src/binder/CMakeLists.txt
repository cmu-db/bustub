add_subdirectory(statement)

add_library(
  bustub_binder
  OBJECT
  binder.cpp
  fmt_impl.cpp
  keyword_helper.cpp
  node_tag_to_string.cpp
  sql_statement.cpp
  transformer.cpp
  )
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_binder>
    PARENT_SCOPE)
