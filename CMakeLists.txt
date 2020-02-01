cmake_minimum_required (VERSION 2.8)

project ("GNSSViewer")

option (GNSSVIEWER_BUILD_EXAMPLES "Build gnssviewer examples" OFF)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set (CMAKE_LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)
set (CMAKE_INCLUDE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/include)

set (gnssviewer_include_dirs ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories (SYSTEM ${gnssviewer_include_dirs})

macro (add_sources)
  file (RELATIVE_PATH _relPath "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
  foreach (_src ${ARGN})
    if (_relPath)
      list (APPEND SRCS "${_relPath}/${_src}")
    else()
      list (APPEND SRCS "${_src}")
    endif ()
  endforeach ()
  if (_relPath)
    # propagate SRCS to parent directory
    set (SRCS ${SRCS} PARENT_SCOPE)
  endif ()
endmacro ()


add_subdirectory (websocket)
add_subdirectory (nmeaparser)

aux_source_directory (gnssviewer src_MAIN)

add_executable (${PROJECT_NAME}
  ${src_MAIN}
)
target_link_libraries (${PROJECT_NAME}
  websocket
  nmeaparser
)
# set_target_properties(${PROJECT_NAME} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${gnssviewer_include_dirs})

if (GNSSVIEWER_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif (GNSSVIEWER_BUILD_EXAMPLES)

