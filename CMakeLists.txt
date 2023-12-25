#######################################################################################################################
##! \file
##! \author Pélégrin Marius
##! \copyright The MIT License (MIT)
##! \date 2022-2023
#######################################################################################################################

# Global project definitions

cmake_minimum_required(VERSION 3.21)

project(Diskon)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Add external dependencies

option(DISKON_ADD_DEPENDENCIES "Add external dependencies to the project" ON)

if(DISKON_ADD_DEPENDENCIES)

    set(RUC_ADD_EXAMPLES OFF)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/external/Ruc)

endif()

# Diskon

add_library(
    diskon SHARED
    ${CMAKE_CURRENT_LIST_DIR}/src/Core/Stream.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Format/FormatStream.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Format/Deflate.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Format/Zlib.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Format/Png.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Format/Riff.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Format/Wave.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Format/Xml.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Format/Pnm.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Format/Obj.cpp
)

target_include_directories(
    diskon
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/external/Ruc/include
)

target_compile_definitions(
    diskon
    PUBLIC DISKON_EXPORTS
)

# Diskon examples

option(DISKON_ADD_EXAMPLES "Add target diskon-examples" ON)

if(DISKON_ADD_EXAMPLES)

    add_executable(
        diskon-examples
        ${CMAKE_CURRENT_LIST_DIR}/examples/main.cpp
    )

    add_dependencies(
        diskon-examples
        diskon
    )

    target_include_directories(
        diskon-examples
        PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
        PUBLIC ${CMAKE_CURRENT_LIST_DIR}/external/Ruc/include
    )

    target_link_libraries(
        diskon-examples
        diskon
    )

endif()
