#
#     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
#
project(FastVector)

set(PROJECT_NAME FastVector)
set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION "An implementation of a fast vector for mathematical operations")

set(PROJECT_CXX_STANDARD 17)

# cmake min version
cmake_minimum_required(VERSION 3.1)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/auto_included")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# CFLAGS
if(NOT CMAKE_BUILD_TYPE)
    #set(CMAKE_BUILD_TYPE "DEBUG")
    #set(CMAKE_BUILD_TYPE "RELEASE")
    set(CMAKE_BUILD_TYPE "RELWITHDEBINFO")
    #set(CMAKE_BUILD_TYPE "MINSIZEREL")
endif()

# output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# load auto_included
file(GLOB_RECURSE cmake_helpers ${CMAKE_CURRENT_LIST_DIR}/auto_included/*.cmake)
foreach(cmake_helper ${cmake_helpers})
    include(${cmake_helper})
endforeach()

set(CMAKE_CXX_STANDARD ${PROJECT_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ForceAVX "Pass mavx to the compiler." ON)
option(NativeOptimization "Pass march=native to the compiler." ON)

set(CMAKE_CXX_COMMON_APPEND "${CMAKE_CXX_FLAGS_C11} -D__STRICT_ANSI__ -Wno-unknown-pragmas -Wconversion")
if (NativeOptimization)
    message(STATUS "Passing match=native to the compiler.")
    set(CMAKE_CXX_COMMON_APPEND "${CMAKE_CXX_COMMON_APPEND} -march=native -fms-extensions")
endif()

if (ForceAVX)
    message("Passing mavx and mavx2 to the compiler.")
    set(CMAKE_CXX_COMMON_APPEND "${CMAKE_CXX_COMMON_APPEND} -mavx2 -mavx")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_COMMON_APPEND}")
set(CMAKE_CXX_FLAGS_VALGRIND "-O3 -Wa,--gstabs -save-temps")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG_BUILD -O2 -Wall -g -ggdb3 -gdwarf-4 -fsanitize=address -fsanitize=undefined -fsanitize=leak -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG_BUILD -O3" )
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -DDEBUG_BUILD -g -ggdb3 -g3")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}") 
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

if(CMAKE_COMPILER_IS_GNUCXX)
    # These would be super pedantic!!
    #add_definitions("-Wall -Wextra -Wshadow -Weffc++ -pedantic -Wstrict-aliasing -Wpointer-arith -Wcast-qual")
    #add_definitions("-Wall -Wno-shadow -Wstrict-aliasing -Wpointer-arith -Wno-cast-qual -Wno-comment")
    add_definitions("-Wno-reorder -Wno-invalid-offsetof -Wno-deprecated-declarations")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlibc++")
    message(WARNING "Some features such as profiling are not supported using Clang")
else()
    message(FATAL_ERROR "CMakeLists.txt has not been tested/written for your compiler.")
endif()

option(BuildUnitTest "Determine whether to build unit tests." ON)

#
# Enabling testing if we find CxxTest
#
find_package(CxxTest)

if (CXXTEST_FOUND)
    message(STATUS "Enabling unit testing.")
    include_directories(${CXXTEST_INCLUDE_DIR})

    # set the cxxtest template that should be used!
    set(CXXTEST_TESTGEN_ARGS --template=${CMAKE_CURRENT_SOURCE_DIR}/cxxtest/cxxtest_template.tpl)

    enable_testing()
else()
    message(STATUS "Cxxtest not found!")
endif()

add_subdirectory(allocator)
add_subdirectory(linearAlgebra)
add_subdirectory(concepts)
add_subdirectory(utils)

include_directories(${INCLUDES})

add_library(VectorHelpers SHARED "${SRCS}")

set_target_properties(VectorHelpers PROPERTIES VERSION ${PROJECT_VERSION})


