
add_executable(${PROJECT_NAME}_simple_table
    1-simple_table.c)
target_link_libraries(${PROJECT_NAME}_simple_table
    fort)

add_executable(${PROJECT_NAME}_custom_table
    2-custom_table.c)
target_link_libraries(${PROJECT_NAME}_custom_table
    fort)

add_executable(${PROJECT_NAME}_complex_layout
    3-complex_layout.c)
target_link_libraries(${PROJECT_NAME}_complex_layout
    fort)

add_executable(${PROJECT_NAME}_fill_table
    4-fill_table.c)
target_link_libraries(${PROJECT_NAME}_fill_table
    fort)

add_executable(${PROJECT_NAME}_beautiful_table
    5-beautiful_table.c)
target_link_libraries(${PROJECT_NAME}_beautiful_table
    fort)

add_executable(${PROJECT_NAME}_print_styles
    6-print_styles.c)
target_link_libraries(${PROJECT_NAME}_print_styles
    fort)

add_executable(${PROJECT_NAME}_custom_border_style
    7-custom_border_style.c)
target_link_libraries(${PROJECT_NAME}_custom_border_style
    fort)

add_executable(${PROJECT_NAME}_math_table
    8-math_table.c)
target_link_libraries(${PROJECT_NAME}_math_table
    fort)

add_executable(${PROJECT_NAME}_non_ascii_table
    9-non_ascii_table.c)
target_link_libraries(${PROJECT_NAME}_non_ascii_table
    fort)




add_executable(${PROJECT_NAME}_simple_table_cpp
    1-simple_table.cpp)
target_link_libraries(${PROJECT_NAME}_simple_table_cpp
    fort)

add_executable(${PROJECT_NAME}_custom_table_cpp
    2-custom_table.cpp)
target_link_libraries(${PROJECT_NAME}_custom_table_cpp
    fort)

add_executable(${PROJECT_NAME}_complex_layout_cpp
    3-complex_layout.cpp)
target_link_libraries(${PROJECT_NAME}_complex_layout_cpp
    fort)

add_executable(${PROJECT_NAME}_fill_table_cpp
    4-fill_table.cpp)
target_link_libraries(${PROJECT_NAME}_fill_table_cpp
    fort)

add_executable(${PROJECT_NAME}_beautiful_table_cpp
    5-beautiful_table.cpp)
target_link_libraries(${PROJECT_NAME}_beautiful_table_cpp
    fort)

add_executable(${PROJECT_NAME}_non_ascii_table_cpp
    9-non_ascii_table.cpp)
target_link_libraries(${PROJECT_NAME}_non_ascii_table_cpp
    fort)


set(${PROJECT_NAME}_examples
    ${PROJECT_NAME}_simple_table
    ${PROJECT_NAME}_custom_table
    ${PROJECT_NAME}_fill_table
    ${PROJECT_NAME}_custom_border_style
    ${PROJECT_NAME}_print_styles
    ${PROJECT_NAME}_math_table
    ${PROJECT_NAME}_beautiful_table
    ${PROJECT_NAME}_complex_layout
    ${PROJECT_NAME}_non_ascii_table

    ${PROJECT_NAME}_simple_table_cpp
    ${PROJECT_NAME}_custom_table_cpp
    ${PROJECT_NAME}_complex_layout_cpp
    ${PROJECT_NAME}_fill_table_cpp
    ${PROJECT_NAME}_beautiful_table_cpp
    ${PROJECT_NAME}_non_ascii_table_cpp
    PARENT_SCOPE)

if(DEFINED FORT_LINK_LIBRARIES)
    foreach(exe ${${PROJECT_NAME}_examples})
        target_link_libraries(${exe} "${FORT_LINK_LIBRARIES}")
    endforeach()
endif()
