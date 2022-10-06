option(FORT_HAVE_WCHAR "Enable wchar support" ON)
option(FORT_HAVE_UTF8 "Enable UTF8 support" ON)

add_library(fort
    fort.c)
add_library(${PROJECT_NAME}::fort ALIAS fort)

target_include_directories(fort
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(fort
    PROPERTIES
        VERSION ${libfort_VERSION}
        SOVERSION ${libfort_SOVERSION}
        POSITION_INDEPENDENT_CODE ON
)

if(NOT FORT_HAVE_WCHAR)
    target_compile_definitions(fort
        PRIVATE
            -DFT_CONGIG_DISABLE_WCHAR
    )
endif()

if(NOT FORT_HAVE_UTF8)
    target_compile_definitions(fort
        PRIVATE
            -DFT_CONGIG_DISABLE_UTF8
    )
endif()


include(GNUInstallDirs)
install(
    TARGETS fort
    EXPORT ${PROJECT_NAME}-targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(
    FILES fort.h fort.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

configure_file(libfort.pc.in libfort.pc @ONLY)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/libfort.pc
    DESTINATION lib/pkgconfig
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/${PROJECT_NAME}-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    COMPATIBILITY SameMajorVersion
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
    EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}-targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

