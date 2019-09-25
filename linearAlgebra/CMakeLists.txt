#
#     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
#
add_includes(${CMAKE_CURRENT_SOURCE_DIR})

add_sources(Vector.cpp)

if (CXXTEST_FOUND)
    add_subdirectory(tests)
endif()
