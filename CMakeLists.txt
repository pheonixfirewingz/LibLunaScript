cmake_minimum_required(VERSION 3.22.1)
project(liblunascript C CXX)
#options
set(BUILD_DOCS OFF)
set(ENABLE_REGRESSION_TESTS ON)
#danger using this option as it could damage tests that are working as expected do
# a regression test first and check to see if the out put is as expected
set(REGEN_TESTS_REGRESSION_TESTS OFF)
set(STATIC_BUILD OFF)
set(TERMIANL_MODE ON)
#options end

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include(FlagControl)
include(LocalLibrary)

add_definitions(-DPROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

if(${REGEN_TESTS_REGRESSION_TESTS})
    add_definitions(-DGEN_TESTS)
endif()

# Debug mode by default.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DWITH_DEBUG)
endif()

if(${TERMIANL_MODE})
    add_definitions(-DTERMIANL_MODE)
endif()

if(${ENABLE_REGRESSION_TESTS})
    add_definitions(-DENABLE_TESTS)
endif()

if(CMAKE_SYSTEM_NAME MATCHES Linux)
    message(INFO "[SOFTRENDER] - building for Linux....")
    add_definitions(-DON_LINUX)
    set(ON_LINUX ON)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif(CMAKE_SYSTEM_NAME MATCHES WindowsStore)
    message(INFO "[SOFTRENDER] -  building for WindowsStore....")
   add_definitions(-DON_UWP)
   set(ON_UWP ON)
   set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif(CMAKE_SYSTEM_NAME MATCHES Windows)
    message(INFO "[SOFTRENDER] - building for Windows....")
    add_definitions(-DON_WINDOWS)
    set(ON_WINDOWS ON)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif(CMAKE_SYSTEM_NAME MATCHES Android)
    message(INFO "[SOFTRENDER] - building for Android....")
    add_definitions(-DON_ANDROID)
    if(${STATIC_BUILD})
        set(STATIC_BUILD OFF)
    endif()
    set(ON_ANDROID ON)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
else()
    message(FATAL_ERROR "system not supported")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
    add_definitions(-DIS_CLANG)
elseif (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    if(CMAKE_C_COMPILER_VERSION VERSION_LESS 12)
        message(FATAL_ERROR "anything lower that gcc-12 compiler not supported")
    endif()
    add_definitions(-DIS_GCC)
elseif (CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    add_definitions(-DIS_MSVC)
else()
    message(FATAL_ERROR "compiler not supported")
endif()

add_local_library(libos shared)
add_library(${CMAKE_PROJECT_NAME}  SHARED ${PROJECT_SOURCE_DIR}/src/API.cpp)
flagsSetup(${CMAKE_PROJECT_NAME})

target_include_directories (${CMAKE_PROJECT_NAME} PRIVATE  ${PROJECT_SOURCE_DIR}/libs/include STATIC)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/Includes STATIC)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src STATIC)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE  libos)
include(SourceList)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

if(${ENABLE_REGRESSION_TESTS})
    add_executable(LunaScript  ${PROJECT_SOURCE_DIR}/Main.cpp)
else()
    add_executable(LunaScript  ${PROJECT_SOURCE_DIR}/Main_Release.cpp)
endif()

target_include_directories (LunaScript PRIVATE  ${PROJECT_SOURCE_DIR}/libs/include STATIC)
target_include_directories (LunaScript PRIVATE  ${PROJECT_SOURCE_DIR}/src/API STATIC)
flagsSetup(LunaScript)

if(${ENABLE_REGRESSION_TESTS})
    include ( FetchContent )
    include ( GoogleTest )
    FetchContent_Declare (
        googletest
        URL https://github.com/google/googletest/archive/58d77fa8070e8cec2dc1ed015d66b454c8d78850.zip
    )
    set ( gtest_force_shared_crt ON CACHE BOOL "" FORCE )
    FetchContent_MakeAvailable ( googletest )
    enable_testing ()
    gtest_discover_tests ( LunaScript )
    target_link_libraries(LunaScript PRIVATE GTest::gtest_main ${CMAKE_PROJECT_NAME} libos)
else()
    target_link_libraries(LunaScript PRIVATE ${CMAKE_PROJECT_NAME} libos)
endif()