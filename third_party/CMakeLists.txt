add_subdirectory(murmur3)
add_subdirectory(libpg_query)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)  # don't override our compiler/linker options when building gtest
add_subdirectory(googletest)
