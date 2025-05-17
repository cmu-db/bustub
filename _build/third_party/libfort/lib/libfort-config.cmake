if(NOT TARGET libfort::fort)
    include("${CMAKE_CURRENT_LIST_DIR}/libfort-targets.cmake")
endif()

set(libfort_LIBRARIES libfort::fort)
