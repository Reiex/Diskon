#######################################################################################################################
##! \file
##! \author Pélégrin Marius
##! \copyright The MIT License (MIT)
##! \date 2022-2023
#######################################################################################################################

cmake_minimum_required(VERSION 3.21)

project(Diskon)

set(CMAKE_CXX_STANDARD 23)


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


add_executable(
    diskon-examples
    ${CMAKE_CURRENT_LIST_DIR}/examples/main.cpp
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


add_dependencies(
    diskon-examples
    diskon
)
