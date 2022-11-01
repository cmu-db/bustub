add_library(
  bustub_statement
  OBJECT
  create_statement.cpp
  delete_statement.cpp
  explain_statement.cpp
  index_statement.cpp
  insert_statement.cpp
  select_statement.cpp
  update_statement.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_statement>
  PARENT_SCOPE)
