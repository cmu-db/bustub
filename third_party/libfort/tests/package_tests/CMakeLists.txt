cmake_minimum_required(VERSION 3.0)

project(foo)

find_package(libfort)

# Assert exports of libfort
if (NOT libfort_FOUND)
	message(FATAL_ERROR "libfort was not found")
endif()
if (NOT TARGET libfort::fort)
	message(FATAL_ERROR "TARGET libfort::fort not found")
endif()
if (NOT DEFINED libfort_LIBRARIES)
	message(FATAL_ERROR "libfort_LIBRARIES are not defined")
endif()
if (NOT ${libfort_VERSION} EQUAL "0.5.0")
	message(FATAL_ERROR "libfort_VERSION is incorrect")
endif()

# Create target that depend on libfort and check they'll compile ok
add_executable(foo-app
    foo-app.cpp)
target_link_libraries(foo-app
	PRIVATE libfort::fort
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

