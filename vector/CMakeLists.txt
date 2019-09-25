#
#     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
#

add_includes(${CMAKE_CURRENT_SOURCE_DIR})

if (CXXTEST_FOUND)
    add_subdirectory(tests)
endif()
