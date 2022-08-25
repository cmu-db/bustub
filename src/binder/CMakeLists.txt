include_directories(../../third_party/libpg_query/include)
include_directories(../../third_party/libpg_query)

add_subdirectory(statement)

add_library(
  bustub_binder
  OBJECT
  keyword_helper.cpp
  sql_statement.cpp
  nodetype_to_string.cpp
  parser.cpp
  transformer.cpp)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_binder>
    PARENT_SCOPE)
