option(FORT_HAVE_WCHAR "Enable wchar support" ON)
option(FORT_HAVE_UTF8 "Enable UTF8 support" ON)
option(FORT_TEST_BUILD "Export some internal symbols for tests" ON)


add_executable(${PROJECT_NAME}_test_dev
    main_test.c
    wb_tests/test_vector.c
    wb_tests/test_string_buffer.c
    wb_tests/test_table_geometry.c
    bb_tests/test_table_basic.c
    bb_tests/test_table_border_style.c
    bb_tests/test_table_properties.c
    bb_tests/test_memory_errors.c
    bb_tests/test_error_codes.c
    tests.c
    test_utils.c)
target_link_libraries(${PROJECT_NAME}_test_dev
    fort_dev)
target_compile_definitions(${PROJECT_NAME}_test_dev
    PUBLIC FORT_WB_TESTING_ENABLED=1)
target_include_directories(${PROJECT_NAME}_test_dev
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)


add_executable(${PROJECT_NAME}_test
    main_test.c
    bb_tests/test_table_basic.c
    bb_tests/test_table_border_style.c
    bb_tests/test_table_properties.c
    bb_tests/test_memory_errors.c
    bb_tests/test_error_codes.c
    tests.c
    test_utils.c)
target_link_libraries(${PROJECT_NAME}_test
    fort)
target_include_directories(${PROJECT_NAME}_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)


add_executable(${PROJECT_NAME}_test_cpp
    main_test_cpp.cpp
    bb_tests_cpp/test_table_basic.cpp
    bb_tests_cpp/test_table_properties.cpp
    tests.c
    test_utils.cpp)
target_link_libraries(${PROJECT_NAME}_test_cpp
    fort)
target_include_directories(${PROJECT_NAME}_test_cpp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set(${PROJECT_NAME}_tests
    ${PROJECT_NAME}_test_dev
    ${PROJECT_NAME}_test_cpp
    ${PROJECT_NAME}_test
    PARENT_SCOPE)

if(DEFINED FORT_LINK_LIBRARIES)
    foreach(exe ${${PROJECT_NAME}_tests})
        target_link_libraries(${exe} "${FORT_LINK_LIBRARIES}")
    endforeach()
endif()
