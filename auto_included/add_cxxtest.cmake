#
#     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
#
macro(CXXTEST TestName)
    CXXTEST_ADD_TEST(${TestName} ${TestName}.cc ${CMAKE_CURRENT_SOURCE_DIR}/${TestName}.h)
    target_link_libraries(${TestName} VectorHelpers ${CMAKE_THREAD_LIBS_INIT})

    # TODO fix this some other way
    include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/utils 
        ${PROJECT_SOURCE_DIR}/allocator
        ${PROJECT_SOURCE_DIR}/linearAlgebra
        ${PROJECT_SOURCE_DIR}/concepts
        ${PROJECT_SOURCE_DIR}/vector)

    set_property(SOURCE ${TestName}.cc PROPERTY SKIP_AUTOGEN ON)
endmacro(CXXTEST)


