add_library(
  bustub_planner
  OBJECT
  expression_factory.cpp
  plan_aggregation.cpp
  plan_func_call.cpp
  plan_expression.cpp
  plan_insert.cpp
  plan_table_ref.cpp
  plan_select.cpp
  planner.cpp)

set(ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_planner>
  PARENT_SCOPE)
