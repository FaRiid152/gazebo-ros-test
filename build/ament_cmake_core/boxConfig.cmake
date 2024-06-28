# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_box_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED box_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(box_FOUND FALSE)
  elseif(NOT box_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(box_FOUND FALSE)
  endif()
  return()
endif()
set(_box_CONFIG_INCLUDED TRUE)

# output package information
if(NOT box_FIND_QUIETLY)
  message(STATUS "Found box: 0.0.0 (${box_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'box' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT ${box_DEPRECATED_QUIET})
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(box_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${box_DIR}/${_extra}")
endforeach()
