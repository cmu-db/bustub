cmake_minimum_required(VERSION 3.0)

project(foo)

set(FORT_ENABLE_TESTING OFF CACHE INTERNAL "")
add_subdirectory(libfort)

# Create target that depend on libfort and check they'll compile ok
add_executable(foo-app
    foo-app.cpp)
target_link_libraries(foo-app
	PRIVATE fort
)

add_library(foo-lib
	foo-lib.cpp)
target_link_libraries(foo-lib
	PRIVATE libfort::fort)

add_library(foo-lib-shared SHARED
	foo-lib.cpp)
target_link_libraries(foo-lib-shared
	PRIVATE libfort::fort)

include(GNUInstallDirs)
install(
	TARGETS foo-lib foo-lib-shared
	EXPORT ${PROJECT_NAME}-targets
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


include(CMakePackageConfigHelpers)

install(
	EXPORT ${PROJECT_NAME}-targets
	FILE ${PROJECT_NAME}-targets.cmake
	NAMESPACE ${PROJECT_NAME}::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

