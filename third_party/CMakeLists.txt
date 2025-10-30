add_subdirectory(murmur3)
add_subdirectory(libpg_query)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # don't override our compiler/linker options when building gtest
# Prevent system googletest headers from being used - add our include paths with higher priority
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/third_party/googletest/googletest/include)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/third_party/googletest/googlemock/include)
add_subdirectory(googletest)

add_subdirectory(fmt)
add_subdirectory(linenoise)

set(FORT_ENABLE_TESTING OFF CACHE INTERNAL "")
add_subdirectory(libfort)

add_subdirectory(argparse)

add_subdirectory(utf8proc)

add_subdirectory(backward-cpp)

add_subdirectory(readerwriterqueue)
