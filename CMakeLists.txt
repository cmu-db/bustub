cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # For clang-tidy.
set(BUILD_SHARED_LIBS OFF) # We expect external libraries to be linked statically.
set(CMAKE_CXX_STANDARD 17) # Compile as C++17.
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Require C++17 support.

project(BusTub
        VERSION 2023.1
        DESCRIPTION "The BusTub Relational Database Management System (Educational) @ https://github.com/cmu-db/bustub"
        LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        message(STATUS "Setting build type to `Debug` as none was specified.")
        set(CMAKE_BUILD_TYPE "Debug")
endif()

if(EMSCRIPTEN)
        add_compile_options(-fexceptions)
        add_link_options(-fexceptions)
endif()

# People keep running CMake in the wrong folder, completely nuking their project or creating weird bugs.
# This checks if you're running CMake from a folder that already has CMakeLists.txt.
# Importantly, this catches the common case of running it from the root directory.
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" PATH_TO_CMAKELISTS_TXT)

if(EXISTS "${PATH_TO_CMAKELISTS_TXT}")
        message(FATAL_ERROR "Run CMake from a build subdirectory! \"mkdir build ; cd build ; cmake ..\" \
    Some junk files were created in this folder (CMakeCache.txt, CMakeFiles); you should delete those.")
endif()

# Expected directory structure.
set(BUSTUB_BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build_support")
set(BUSTUB_CLANG_SEARCH_PATH "/usr/local/bin" "/usr/bin" "/usr/local/opt/llvm/bin" "/usr/local/opt/llvm@14/bin"
        "/opt/homebrew/opt/llvm@14/bin/")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if(CMAKE_CXX_COMPILER_VERSION MATCHES "^14.")
                message(STATUS "You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
        else()
                message(WARNING "!! We recommend that you use clang-14 for developing BusTub. You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}, a different version.")
        endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        message(STATUS "You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
else()
        message(WARNING "!! We recommend that you use clang-14 for developing BusTub. You're using ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}, which is not clang.")
endif()

# #####################################################################################################################
# DEPENDENCIES
# #####################################################################################################################

# CTest
enable_testing()

# clang-format

# attempt to find the binary if user did not specify
find_program(CLANG_FORMAT_BIN
        NAMES clang-format clang-format-14
        HINTS ${BUSTUB_CLANG_SEARCH_PATH})

if("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
        message(WARNING "BusTub/main couldn't find clang-format.")
else()
        message(STATUS "BusTub/main found clang-format at ${CLANG_FORMAT_BIN}")
endif()

# attempt to find the binary if user did not specify
find_program(CLANG_TIDY_BIN
        NAMES clang-tidy clang-tidy-14
        HINTS ${BUSTUB_CLANG_SEARCH_PATH})

if("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
        message(WARNING "BusTub/main couldn't find clang-tidy.")
else()
        # Output compile_commands.json
        set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
        message(STATUS "BusTub/main found clang-tidy at ${CLANG_TIDY_BIN}")
endif()

find_program(CLANG_APPLY_REPLACEMENTS_BIN
        NAMES clang-apply-replacements clang-apply-replacements-14
        HINTS ${BUSTUB_CLANG_SEARCH_PATH})

if("${CLANG_APPLY_REPLACEMENTS_BIN}" STREQUAL "CLANG_APPLY_REPLACEMENTS_BIN-NOTFOUND")
        message(WARNING "BusTub/main couldn't find clang-apply-replacements.")
else()
        # Output compile_commands.json
        set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
        message(STATUS "BusTub/main found clang-apply-replacements at ${CLANG_APPLY_REPLACEMENTS_BIN}")
endif()

# cpplint
find_program(CPPLINT_BIN
        NAMES cpplint cpplint.py
        HINTS "${BUSTUB_BUILD_SUPPORT_DIR}")

if("${CPPLINT_BIN}" STREQUAL "CPPLINT_BIN-NOTFOUND")
        message(WARNING "BusTub/main couldn't find cpplint.")
else()
        message(STATUS "BusTub/main found cpplint at ${CPPLINT_BIN}")
endif()

# #####################################################################################################################
# COMPILER SETUP
# #####################################################################################################################
if(NOT DEFINED BUSTUB_SANITIZER)
        set(BUSTUB_SANITIZER address)
endif()

message("Build mode: ${CMAKE_BUILD_TYPE}")
message("${BUSTUB_SANITIZER} sanitizer will be enabled in debug mode.")

# Compiler flags.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-unused-parameter -Wno-attributes") # TODO: remove
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fsanitize=${BUSTUB_SANITIZER} -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")

# Output directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Includes.
set(BUSTUB_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
set(BUSTUB_TEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/test/include)
set(BUSTUB_THIRD_PARTY_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}/third_party
        ${PROJECT_SOURCE_DIR}/third_party/fmt/include
        ${PROJECT_SOURCE_DIR}/third_party/libpg_query/include
        ${PROJECT_SOURCE_DIR}/third_party/argparse/include
        ${PROJECT_SOURCE_DIR}/third_party/cpp_random_distributions
        ${PROJECT_SOURCE_DIR}/third_party/backward-cpp/include
)

include_directories(${BUSTUB_SRC_INCLUDE_DIR} ${BUSTUB_TEST_INCLUDE_DIR} ${BUSTUB_THIRD_PARTY_INCLUDE_DIR})
include_directories(BEFORE src) # This is needed for gtest.

function(disable_target_warnings NAME)
        target_compile_options(${NAME} PRIVATE "-w")
endfunction()

# #####################################################################################################################
# Other CMake modules
# MUST BE ADDED AFTER CONFIGURING COMPILER PARAMETERS
# #####################################################################################################################
set(CMAKE_MODULE_PATH "${BUSTUB_BUILD_SUPPORT_DIR}/cmake;${CMAKE_MODULE_PATH}")
find_package(LibElf)
find_package(LibDwarf)

add_subdirectory(third_party)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(tools)

# #####################################################################################################################
# MAKE TARGETS
# #####################################################################################################################

# #########################################
# "make format"
# "make check-format"
# #########################################
string(CONCAT BUSTUB_FORMAT_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/src,"
        "${CMAKE_CURRENT_SOURCE_DIR}/test,"
)

# Runs clang format and updates files in place.
add_custom_target(format ${BUSTUB_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${BUSTUB_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${BUSTUB_FORMAT_DIRS}
        --fix
        --quiet
)

# Runs clang format and exits with a non-zero exit code if any files need to be reformatted
add_custom_target(check-format ${BUSTUB_BUILD_SUPPORT_DIR}/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${BUSTUB_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
        --source_dirs
        ${BUSTUB_FORMAT_DIRS}
        --quiet
)

# #########################################
# "make check-lint"
# #########################################
file(GLOB_RECURSE BUSTUB_LINT_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
)

# Balancing act: cpplint.py takes a non-trivial time to launch,
# so process 12 files per invocation, while still ensuring parallelism
add_custom_target(check-lint echo '${BUSTUB_LINT_FILES}' | xargs -n12 -P8
        ${CPPLINT_BIN}
        --verbose=2 --quiet
        --linelength=120
        --filter=-legal/copyright,-build/header_guard,-runtime/references # https://github.com/cpplint/cpplint/issues/148
)

# ##########################################################
# "make check-clang-tidy" target
# ##########################################################
# runs clang-tidy and exits with a non-zero exit code if any errors are found.
# note that clang-tidy automatically looks for a .clang-tidy file in parent directories
add_custom_target(check-clang-tidy
        ${BUSTUB_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
)
add_custom_target(fix-clang-tidy
        ${BUSTUB_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
        -clang-apply-replacements-binary ${CLANG_APPLY_REPLACEMENTS_BIN} # using our clang-apply-replacements binary
        -fix # apply suggested changes generated by clang-tidy
)
add_custom_target(check-clang-tidy-diff
        ${BUSTUB_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
        -only-diff # only check diff files to master
)
add_custom_target(fix-clang-tidy-diff
        ${BUSTUB_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
        -clang-apply-replacements-binary ${CLANG_APPLY_REPLACEMENTS_BIN} # using our clang-apply-replacements binary
        -fix # apply suggested changes generated by clang-tidy
        -only-diff # only check diff files to master
)

# ##########################################################
# "make check-clang-tidy" target for projects
# check clang-tidy on the whole projects is slow, so we
# hardcode some files to check here for each project.
# ##########################################################
set(P0_FILES
        "src/include/primer/trie_answer.h"
        "src/include/primer/trie_store.h"
        "src/include/primer/trie.h"
        "src/primer/trie_store.cpp"
        "src/primer/trie.cpp"
        "src/planner/plan_func_call.cpp"
        "src/include/execution/expressions/string_expression.h"
)
add_custom_target(check-clang-tidy-p0
        ${BUSTUB_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
        ${P0_FILES}
)
add_custom_target(submit-p0
        zip project0-submission.zip
        ${P0_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set(P1_FILES
        "src/include/buffer/lru_k_replacer.h"
        "src/buffer/lru_k_replacer.cpp"
        "src/include/buffer/buffer_pool_manager.h"
        "src/buffer/buffer_pool_manager.cpp"
        "src/include/storage/disk/disk_scheduler.h"
        "src/storage/disk/disk_scheduler.cpp"
)
add_custom_target(check-clang-tidy-p1
        ${BUSTUB_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
        ${P1_FILES}
)
add_custom_target(submit-p1
        zip project1-submission.zip
        ${P1_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set(P2_FILES
        "src/include/storage/page/page_guard.h"
        "src/storage/page/page_guard.cpp"
        "src/include/storage/page/extendible_htable_bucket_page.h"
        "src/storage/page/extendible_htable_bucket_page.cpp"
        "src/include/storage/page/extendible_htable_directory_page.h"
        "src/storage/page/extendible_htable_directory_page.cpp"
        "src/include/storage/page/extendible_htable_header_page.h"
        "src/storage/page/extendible_htable_header_page.cpp"
        "src/include/container/disk/hash/disk_extendible_hash_table.h"
        "src/container/disk/hash/disk_extendible_hash_table.cpp"
        ${P1_FILES}
)
add_custom_target(check-clang-tidy-p2
        ${BUSTUB_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
        ${P2_FILES}
)
add_custom_target(submit-p2
        zip project2-submission.zip
        ${P2_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set(P3_FILES
        "src/include/execution/executors/aggregation_executor.h"
        "src/include/execution/executors/delete_executor.h"
        "src/include/execution/executors/filter_executor.h"
        "src/include/execution/executors/hash_join_executor.h"
        "src/include/execution/executors/index_scan_executor.h"
        "src/include/execution/executors/insert_executor.h"
        "src/include/execution/executors/limit_executor.h"
        "src/include/execution/executors/nested_loop_join_executor.h"
        "src/include/execution/executors/seq_scan_executor.h"
        "src/include/execution/executors/sort_executor.h"
        "src/include/execution/executors/topn_executor.h"
        "src/include/execution/executors/topn_per_group_executor.h"
        "src/include/execution/executors/update_executor.h"
        "src/include/execution/executors/window_function_executor.h"
        "src/execution/aggregation_executor.cpp"
        "src/execution/window_function_executor.cpp"
        "src/execution/delete_executor.cpp"
        "src/execution/filter_executor.cpp"
        "src/execution/hash_join_executor.cpp"
        "src/execution/index_scan_executor.cpp"
        "src/execution/insert_executor.cpp"
        "src/execution/limit_executor.cpp"
        "src/execution/nested_loop_join_executor.cpp"
        "src/execution/seq_scan_executor.cpp"
        "src/execution/sort_executor.cpp"
        "src/execution/topn_executor.cpp"
        "src/execution/topn_per_group_executor.cpp"
        "src/execution/update_executor.cpp"
        "src/include/optimizer/optimizer.h"
        "src/include/optimizer/optimizer_internal.h"
        "src/optimizer/nlj_as_hash_join.cpp"
        "src/optimizer/optimizer_custom_rules.cpp"
        "src/optimizer/sort_limit_as_topn.cpp"
        "src/optimizer/optimizer_internal.cpp"
        "src/optimizer/seqscan_as_indexscan.cpp"
        "src/common/bustub_ddl.cpp"
        "src/include/execution/plans/topn_per_group_plan.h"
        ${P2_FILES}
)

add_custom_target(check-clang-tidy-p3
        ${BUSTUB_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
        ${P3_FILES}
)
add_custom_target(submit-p3
        zip project3-submission.zip
        ${P3_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set(P4_FILES
        "src/include/concurrency/transaction_manager.h"
        "src/concurrency/transaction_manager.cpp"
        "src/include/concurrency/watermark.h"
        "src/concurrency/watermark.cpp"
        "src/include/execution/execution_common.h"
        "src/execution/execution_common.cpp"
        ${P3_FILES}
)

add_custom_target(check-clang-tidy-p4
        ${BUSTUB_BUILD_SUPPORT_DIR}/run_clang_tidy.py # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN} # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR} # using cmake's generated compile commands
        ${P4_FILES}
)

add_custom_target(submit-p4
        zip project4-submission.zip
        ${P4_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_dependencies(check-clang-tidy gtest bustub) # needs gtest headers, compile_commands.json
