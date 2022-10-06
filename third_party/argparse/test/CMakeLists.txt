cmake_minimum_required(VERSION 3.6)
project(argparse_tests)

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -Wsign-conversion -Wshadow -Wconversion")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Disable deprecation for windows
if (WIN32)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# ARGPARSE executable
file(GLOB ARGPARSE_TEST_SOURCES
    main.cpp
    test_actions.cpp
    test_append.cpp
    test_compound_arguments.cpp
    test_container_arguments.cpp
    test_const_correct.cpp
    test_default_args.cpp
    test_get.cpp
    test_help.cpp
    test_invalid_arguments.cpp
    test_is_used.cpp
    test_issue_37.cpp
    test_negative_numbers.cpp
    test_optional_arguments.cpp
    test_parent_parsers.cpp
    test_parse_args.cpp
    test_positional_arguments.cpp
    test_repr.cpp
    test_required_arguments.cpp
    test_scan.cpp
    test_value_semantics.cpp
    test_version.cpp
    test_subparsers.cpp
    test_parse_known_args.cpp
    test_equals_form.cpp
    test_prefix_chars.cpp
)
set_source_files_properties(main.cpp
    PROPERTIES
    COMPILE_DEFINITIONS DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)
ADD_EXECUTABLE(ARGPARSE_TESTS ${ARGPARSE_TEST_SOURCES})
INCLUDE_DIRECTORIES("../include" ".")
set_target_properties(ARGPARSE_TESTS PROPERTIES OUTPUT_NAME tests)
set_property(TARGET ARGPARSE_TESTS PROPERTY CXX_STANDARD 17)

# Set ${PROJECT_NAME} as the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ARGPARSE_TESTS)
