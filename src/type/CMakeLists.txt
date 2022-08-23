add_library(
    bustub_type
    OBJECT
    bigint_type.cpp
    boolean_type.cpp
    decimal_type.cpp
    integer_parent_type.cpp
    integer_type.cpp
    smallint_type.cpp
    timestamp_type.cpp
    tinyint_type.cpp
    type.cpp
    value.cpp
    varlen_type.cpp)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:bustub_type>
    PARENT_SCOPE)
