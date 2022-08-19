cmake_minimum_required(VERSION 3.0)
# project(bustub_murmur3 CXX C)

# set(CMAKE_CXX_VISIBILITY_PRESET hidden)

add_library(bustub_murmur3 SHARED MurmurHash3.cpp)

# install(TARGETS bustub_murmur3
#         EXPORT "${BUSTUB_EXPORT_SET}"
#         LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
#         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")

# disable_target_warnings(bustub_murmur3)

# set_target_properties(bustub_murmur3 PROPERTIES POSITION_INDEPENDENT_CODE ON)
