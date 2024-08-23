#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libfort::fort" for configuration "Release"
set_property(TARGET libfort::fort APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libfort::fort PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libfort.a"
  )

list(APPEND _cmake_import_check_targets libfort::fort )
list(APPEND _cmake_import_check_files_for_libfort::fort "${_IMPORT_PREFIX}/lib/libfort.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
