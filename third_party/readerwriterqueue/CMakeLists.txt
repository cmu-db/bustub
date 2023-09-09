cmake_minimum_required(VERSION 3.9)
project(readerwriterqueue VERSION 1.0.0)

include(GNUInstallDirs)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(readerwriterqueue INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

install(FILES atomicops.h readerwriterqueue.h readerwritercircularbuffer.h LICENSE.md
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
