cmake_minimum_required(VERSION 3.16.3)
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
include_directories(${PROJECT_SOURCE_DIR}/src/share/rapidjson/include STATIC)
#options
Set(LIB_MODE OFF)
#options end 

set(compiler_source 
src/compiler/Lexer.cpp 
src/compiler/Parser.cpp
src/compiler/Json.cpp
src/compiler/CodeGen.cpp
src/compiler/Main.cpp
)

include(FetchContent)
include(GoogleTest)
FetchContent_Declare(
googletest
URL https://github.com/google/googletest/archive/58d77fa8070e8cec2dc1ed015d66b454c8d78850.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

if(${LIB_MODE})
    add_library(LunaScript SHARED ${compiler_source})
else()
    add_executable(LunaScript ${compiler_source})
    target_link_libraries(LunaScript GTest::gtest_main)
    gtest_discover_tests(LunaScript)
endif()

set_property(TARGET LunaScript PROPERTY CXX_STANDARD 20)
target_include_directories(LunaScript PRIVATE ${PROJECT_SOURCE_DIR}/include STATIC)
target_compile_options(LunaScript PRIVATE -Wall -Wextra -Werror -g3) #-O2
target_link_options(LunaScript PRIVATE -static-libstdc++)
if(${CMAKE_COMPILER_NUMBER} EQUAL 1)
  target_compile_options(LunaScript PRIVATE -lgcc -static-libgcc)
endif()