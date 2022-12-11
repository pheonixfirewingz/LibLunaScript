cmake_minimum_required(VERSION 3.22.0)
project(libLunaScript CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_CXX_EXTENSIONS ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_COMPILER_NUMBER 0)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_COMPILER_NUMBER 1)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_COMPILER_NUMBER 2)
else()
    message(FATAL_ERROR "compiler not supported")
endif()

configure_file(src/Cmake.h.in ../src/Cmake.h @ONLY)
#options
Set(LIB_MODE OFF)
#options end 

set(compiler_source 
src/compiler/Lexer.cpp 
src/compiler/CodeGen.cpp
src/compiler/Main.cpp
)

if(${LIB_MODE})
    add_library(LunaScript SHARED ${compiler_source})
else()
    add_executable(LunaScript ${compiler_source})
endif()

set_property(TARGET LunaScript PROPERTY CXX_STANDARD 20)
target_include_directories(LunaScript PRIVATE ${PROJECT_SOURCE_DIR}/include STATIC)
target_compile_options(LunaScript PRIVATE -Wall -Wextra -Werror -g3)