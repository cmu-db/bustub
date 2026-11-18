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

# backward-cpp's libdwarf backend is incompatible with libdwarf 0.x, use libdw instead.
if(NOT APPLE AND NOT EMSCRIPTEN)
        set(STACK_DETAILS_AUTO_DETECT FALSE CACHE BOOL "" FORCE)
        set(STACK_DETAILS_DW TRUE CACHE BOOL "" FORCE)
endif()
add_subdirectory(backward-cpp)

add_subdirectory(readerwriterqueue)
