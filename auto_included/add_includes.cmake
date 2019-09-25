#
#     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
#     Description: add includes macro
#

macro(add_includes)
    if (NOT DEFINED INCLUDES_VAR)
        set(INCLUDES_VAR INCLUDES)
    endif()

    file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

    foreach (_include ${ARGN})
        if(_relPath)
            set(_includePath ${_relPath})
        else()
            set(_includePath ${_include})
        endif()

        list(APPEND ${INCLUDES_VAR} ${_includePath})
    endforeach()

    if (_relPath)
        # propagate SRCS to parent directory
        set (${INCLUDES_VAR} ${${INCLUDES_VAR}} PARENT_SCOPE)
    endif()

endmacro()
