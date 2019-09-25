#
#     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
#     Description: add includes macro
#

macro(add_sources)
    if (NOT DEFINED SRCS_VAR)
        set(SRCS_VAR SRCS)
    endif()

    file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

    foreach (_src ${ARGN})
        if(_relPath)
            set(_srcPath ${_relPath}/${_src})
        else()
            set(_srcPath ${_src})
        endif()
        list(APPEND ${SRCS_VAR} ${_srcPath})
    endforeach()
    if (_relPath)
        # propagate SRCS to parent directory
        set (${SRCS_VAR} ${${SRCS_VAR}} PARENT_SCOPE)
    endif()
endmacro()


