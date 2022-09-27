cmake_minimum_required(VERSION 3.10)

include(GoogleTest)

file(GLOB_RECURSE BUSTUB_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*/*test.cpp")

# #####################################################################################################################
# MAKE TARGETS
# #####################################################################################################################

# #########################################
# "make check-tests"
# #########################################
add_custom_target(build-tests COMMAND ${CMAKE_CTEST_COMMAND} --show-only)
add_custom_target(check-tests COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

# #########################################
# "make XYZ_test"
# #########################################
foreach (bustub_test_source ${BUSTUB_TEST_SOURCES})
    # Create a human readable name.
    get_filename_component(bustub_test_filename ${bustub_test_source} NAME)
    string(REPLACE ".cpp" "" bustub_test_name ${bustub_test_filename})

    # Add the test target separately and as part of "make check-tests".
    add_executable(${bustub_test_name} EXCLUDE_FROM_ALL ${bustub_test_source})
    add_dependencies(build-tests ${bustub_test_name})
    add_dependencies(check-tests ${bustub_test_name})

    gtest_discover_tests(${bustub_test_name}
            EXTRA_ARGS
            --gtest_color=auto
            --gtest_output=xml:${CMAKE_BINARY_DIR}/test/${bustub_test_name}.xml
            --gtest_catch_exceptions=0
            DISCOVERY_TIMEOUT 120
            PROPERTIES
            TIMEOUT 30
            )

    target_link_libraries(${bustub_test_name} bustub gtest gmock_main)

    # Set test target properties and dependencies.
    set_target_properties(${bustub_test_name}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
            COMMAND ${bustub_test_name}
            )
endforeach (bustub_test_source ${BUSTUB_TEST_SOURCES})
