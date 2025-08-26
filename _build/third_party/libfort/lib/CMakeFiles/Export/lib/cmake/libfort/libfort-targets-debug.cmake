#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libfort::fort" for configuration "Debug"
set_property(TARGET libfort::fort APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libfort::fort PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libfort.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS libfort::fort )
list(APPEND _IMPORT_CHECK_FILES_FOR_libfort::fort "${_IMPORT_PREFIX}/lib/libfort.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
