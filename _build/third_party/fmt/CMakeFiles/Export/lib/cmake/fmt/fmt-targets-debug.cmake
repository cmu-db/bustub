#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "fmt::fmt" for configuration "Debug"
set_property(TARGET fmt::fmt APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(fmt::fmt PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libfmtd.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS fmt::fmt )
list(APPEND _IMPORT_CHECK_FILES_FOR_fmt::fmt "${_IMPORT_PREFIX}/lib/libfmtd.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
