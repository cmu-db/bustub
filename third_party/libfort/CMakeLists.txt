cmake_minimum_required(VERSION 3.0)

project(libfort VERSION 0.5.0)

string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)"
    "\\1.\\2" libfort_SOVERSION
    ${libfort_VERSION})

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

# ------------------------------------------------------------------------------
# Build options
# ------------------------------------------------------------------------------

option(FORT_ENABLE_ASTYLE "Enable astyle" OFF)
option(FORT_ENABLE_WCHAR "Enable wchar support" ON)
option(FORT_ENABLE_UTF8 "Enable utf8 support" ON)
option(FORT_ENABLE_TESTING "Enables building tests and examples" ON)
set(FORT_BUILD_TYPE "common" CACHE STRING "Build type")

set_property(CACHE FORT_BUILD_TYPE PROPERTY STRINGS
    common asan ubsan coveralls)


# Determine compiler (pos. values Clang, GNU, Intel, MSVC, AppleClang...
# (https://cmake.org/cmake/help/v3.0/variable/CMAKE_LANG_COMPILER_ID.html)
if(FORT_CXX_BUILD)
    set(FORT_COMPILER ${CMAKE_CXX_COMPILER_ID})
else()
    set(FORT_COMPILER ${CMAKE_C_COMPILER_ID})
endif()


set(CMAKE_VERBOSE_MAKEFILE ON)

message(STATUS "libfort build options: "
    " wchar support = ${FORT_ENABLE_WCHAR}; "
    " build type = ${FORT_BUILD_TYPE}")

# for fort.hpp
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------------------
# Warnings
# ------------------------------------------------------------------------------

if("${FORT_COMPILER}" STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W4")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W4")
else()
    set(ADDITIONAL_WARNINGS "\
        -Wall \
        -Wextra \
        -Wdouble-promotion \
        -Wshadow \
        -Wformat=2 \
        -Wno-variadic-macros \
        -Wcast-align \
        -Wstrict-aliasing=2 \
        -Wstrict-overflow=5 \
        -Wfloat-equal \
        -Wwrite-strings \
        ")
    if("${FORT_COMPILER}" STREQUAL "GNU")
        set(ADDITIONAL_WARNINGS "${ADDITIONAL_WARNINGS} \
            -Wtrampolines \
            -Wlogical-op")
    endif()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ADDITIONAL_WARNINGS} -Wpedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADDITIONAL_WARNINGS}")
endif()

# ------------------------------------------------------------------------------
# Sanitizers
# ------------------------------------------------------------------------------

if("${FORT_COMPILER}" STREQUAL "GNU" OR "${FORT_COMPILER}" STREQUAL "Clang")
    # asan case
    if(FORT_BUILD_TYPE STREQUAL "asan")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
        if("${FORT_COMPILER}" STREQUAL "GNU")
            set(FORT_LINK_LIBRARIES asan)
        endif()
    endif()

    #ubsan case
    if(FORT_BUILD_TYPE STREQUAL "ubsan")
        # -fno-sanitize-recover option is used to force faild
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
        if("${FORT_COMPILER}" STREQUAL "GNU")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-sanitize-recover")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-sanitize-recover")
            set(FORT_LINK_LIBRARIES ubsan)

        elseif("${FORT_COMPILER}" STREQUAL "Clang")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-sanitize-recover=all")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-sanitize-recover=all")
        endif()
    endif()

    #coveralls case
    if(FORT_BUILD_TYPE STREQUAL "coveralls")
        set(COVERAGE_FLAGS "-g -fprofile-arcs -ftest-coverage")

        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COVERAGE_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_FLAGS}")
        #target_link_libraries(${PROJECT_NAME}_ex_cpp gcov)
    endif()

endif()

# Hack for some gcc versions
if("${FORT_COMPILER}" STREQUAL "GNU"
        AND NOT "${FORT_BUILD_TYPE}" STREQUAL "common")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
endif()

# ------------------------------------------------------------------------------
# Disable NDEBUG for all builds
# ------------------------------------------------------------------------------
foreach(flags_var_to_scrub
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS_MINSIZEREL
        )
    string(REGEX REPLACE "(^| )[/-]D *NDEBUG($| )" " "
        "${flags_var_to_scrub}" "${${flags_var_to_scrub}}")
endforeach()


# ------------------------------------------------------------------------------
# Add wchar and UTF-8 support for the build
# ------------------------------------------------------------------------------
if(NOT FORT_ENABLE_WCHAR)
    add_definitions(-DFT_CONGIG_DISABLE_WCHAR)
endif()
if(NOT FORT_ENABLE_UTF8)
    add_definitions(-DFT_CONGIG_DISABLE_UTF8)
endif()

set(FORT_HAVE_WCHAR "${FORT_ENABLE_WCHAR}" CACHE STRING "fort option")
set(FORT_HAVE_UTF8 "${FORT_ENABLE_UTF8}" CACHE STRING "fort option")



# ------------------------------------------------------------------------------
# Subdirectories
# ------------------------------------------------------------------------------

add_subdirectory(lib)
if(FORT_ENABLE_TESTING)
    add_subdirectory(src)
    add_subdirectory(tests)
    add_subdirectory(examples)
endif()


# ------------------------------------------------------------------------------
# Sources and executables
# ------------------------------------------------------------------------------

file(GLOB_RECURSE FortHeaders
    "lib/*.h"
    "lib/*.hpp"
    "tests/*.h"
    "tests/*.hpp"
    "src/*.h")
add_custom_target(headers SOURCES ${FortHeaders})


# ------------------------------------------------------------------------------
# Set preprocessor macros for all test builds.
# ------------------------------------------------------------------------------
if(FORT_ENABLE_TESTING)
    target_compile_definitions(${PROJECT_NAME}_test_dev PRIVATE FT_TEST_BUILD=1)
    target_compile_definitions(${PROJECT_NAME}_test_cpp PRIVATE FT_TEST_BUILD=1)
    target_compile_definitions(${PROJECT_NAME}_test PRIVATE FT_TEST_BUILD=1)
endif()

# ------------------------------------------------------------------------------
# Setup text formatting via astyle.
# ------------------------------------------------------------------------------

if(FORT_ENABLE_ASTYLE)
    list(APPEND ASTYLE_CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}"
    )

    externalproject_add(
        astyle
        GIT_REPOSITORY      https://github.com/Bareflank/astyle.git
        GIT_TAG             v1.2
        GIT_SHALLOW         1
        CMAKE_ARGS          ${ASTYLE_CMAKE_ARGS}
        PREFIX              ${CMAKE_BINARY_DIR}/external/astyle/prefix
        TMP_DIR             ${CMAKE_BINARY_DIR}/external/astyle/tmp
        STAMP_DIR           ${CMAKE_BINARY_DIR}/external/astyle/stamp
        DOWNLOAD_DIR        ${CMAKE_BINARY_DIR}/external/astyle/download
        SOURCE_DIR          ${CMAKE_BINARY_DIR}/external/astyle/src
        BINARY_DIR          ${CMAKE_BINARY_DIR}/external/astyle/build
    )

    list(APPEND ASTYLE_ARGS
        --style=kr
        --lineend=linux
        --suffix=none
        --pad-oper
        --unpad-paren
        --align-pointer=name
        --align-reference=name
        --indent-switches
        --keep-one-line-statements
        --keep-one-line-blocks
        --pad-header
        --convert-tabs
        --min-conditional-indent=0
        --indent=spaces=4
        ${CMAKE_SOURCE_DIR}/lib/*.h
        ${CMAKE_SOURCE_DIR}/lib/*.hpp
        ${CMAKE_SOURCE_DIR}/lib/*.c
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.hpp
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/tests/*.c
        ${CMAKE_SOURCE_DIR}/tests/*.h
        ${CMAKE_SOURCE_DIR}/tests/*.hpp
        ${CMAKE_SOURCE_DIR}/tests/bb_tests/*.c
        ${CMAKE_SOURCE_DIR}/tests/wb_tests/*.c
        ${CMAKE_SOURCE_DIR}/tests/bb_tests_cpp/*.cpp
    )

    if(NOT WIN32 STREQUAL "1")
        add_custom_target(
            format
            COMMAND ${CMAKE_BINARY_DIR}/bin/astyle ${ASTYLE_ARGS}
            COMMENT "running astyle"
        )
    else()
        add_custom_target(
            format
            COMMAND ${CMAKE_BINARY_DIR}/bin/astyle.exe ${ASTYLE_ARGS}
            COMMENT "running astyle"
        )
    endif()
endif()


# ------------------------------------------------------------------------------
# Add examples and unit tests to ctest driver.
# ------------------------------------------------------------------------------
if(FORT_ENABLE_TESTING)
    enable_testing()
    list(APPEND ${PROJECT_NAME}_ctests
        ${${PROJECT_NAME}_tests}
        ${${PROJECT_NAME}_examples})
    foreach(exe ${${PROJECT_NAME}_ctests})
        add_test(NAME ${exe} COMMAND ${exe})
    endforeach()
endif()

# ------------------------------------------------------------------------------
# Exported targets for outer applications.
# ------------------------------------------------------------------------------
export(
    TARGETS fort
    FILE ${PROJECT_NAME}-targets.cmake
)

