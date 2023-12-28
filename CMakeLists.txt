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

if(NOT TARGET ruc)
    set(RUC_ADD_EXAMPLES OFF)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/external/Ruc)
endif()

# Diskon

add_library(
    diskon SHARED
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Diskon.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/DiskonDecl.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/DiskonTypes.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/Core.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/CoreDecl.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/CoreTypes.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/Float.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/Hash.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/Huffman.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/Int.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/IntSat.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/LookupMultitable.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/Misc.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/Stream.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/templates/Float.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/templates/Hash.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/templates/Huffman.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/templates/Int.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/templates/IntSat.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/templates/LookupMultitable.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/templates/Misc.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Core/templates/Stream.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Format/Format.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Format/FormatDecl.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Format/FormatTypes.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Format/Deflate.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Format/FormatStream.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Format/Obj.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Format/Png.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Format/Pnm.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Format/Riff.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Format/Wave.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Format/Xml.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Format/Zlib.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Format/templates/Png.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Format/templates/Riff.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/Diskon/Format/templates/Wave.hpp
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

add_dependencies(
    diskon
    ruc
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
