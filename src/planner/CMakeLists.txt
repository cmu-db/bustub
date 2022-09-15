add_library(
  bustub_planner
  OBJECT
  plan_aggregation.cpp
  plan_expression.cpp
  plan_insert.cpp
  plan_table_ref.cpp
  plan_select.cpp
  planner.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_planner>
  PARENT_SCOPE)
