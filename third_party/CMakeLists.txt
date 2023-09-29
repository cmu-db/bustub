add_subdirectory(murmur3)
add_subdirectory(libpg_query)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # don't override our compiler/linker options when building gtest
add_subdirectory(googletest)

add_subdirectory(fmt)
add_subdirectory(linenoise)

set(FORT_ENABLE_TESTING OFF CACHE INTERNAL "")
add_subdirectory(libfort)

add_subdirectory(argparse)

add_subdirectory(utf8proc)

add_subdirectory(backward-cpp)

add_subdirectory(readerwriterqueue)
