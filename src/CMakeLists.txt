# This is hacky :(
file(GLOB_RECURSE bustub_sources ${PROJECT_SOURCE_DIR}/src/*/*.cpp ${PROJECT_SOURCE_DIR}/src/*/*/*.cpp)
add_library(bustub_shared SHARED ${bustub_sources})

######################################################################################################################
# THIRD-PARTY SOURCES
######################################################################################################################

# murmur3
file(GLOB_RECURSE murmur3_sources
        ${PROJECT_SOURCE_DIR}/third_party/murmur3/*.cpp ${PROJECT_SOURCE_DIR}/third_party/murmur3/*.h)
add_library(thirdparty_murmur3 SHARED ${murmur3_sources})
target_link_libraries(bustub_shared thirdparty_murmur3)