add_library(
  bustub_planner
  OBJECT
  plan_expression.cpp
  plan_table_ref.cpp
  planner.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_planner>
  PARENT_SCOPE)
