cmake_minimum_required ( VERSION 3.16.3 )
project ( libLunaScript CXX )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )

# options
set ( DEBUG_MODE ON )
set ( FUZZING OFF )
set ( COMPILER_TESTING OFF )
# options end

configure_file ( ${PROJECT_SOURCE_DIR}/src/application/Cmake.h.in ${PROJECT_SOURCE_DIR}/src/application/Cmake.h @ONLY )

set(compiler_source
    ${PROJECT_SOURCE_DIR}/src/compiler/CompilerAPI.cpp
    ${PROJECT_SOURCE_DIR}/src/compiler/front/Parser.cpp
    ${PROJECT_SOURCE_DIR}/src/compiler/back/CodeGen.cpp
    ${PROJECT_SOURCE_DIR}/src/compiler/front/Json.cpp

    ${PROJECT_SOURCE_DIR}/src/assembler/AssemblerAPI.cpp
    ${PROJECT_SOURCE_DIR}/src/assembler/Lexer.cpp

    ${PROJECT_SOURCE_DIR}/src/lsvm/VmAPI.cpp
    ${PROJECT_SOURCE_DIR}/src/lsvm/VirtualMachine.cpp
)

set ( app_source 
${PROJECT_SOURCE_DIR}/src/application/Main.cpp
)

if ( ${COMPILER_TESTING} )
    set ( app_source 
    ${PROJECT_SOURCE_DIR}/src/application/Test.cpp
    )

    include ( FetchContent )
    include ( GoogleTest )
    FetchContent_Declare (
        googletest
        URL https://github.com/google/googletest/archive/58d77fa8070e8cec2dc1ed015d66b454c8d78850.zip
    )
    set ( gtest_force_shared_crt ON CACHE BOOL "" FORCE )
    FetchContent_MakeAvailable ( googletest )
    enable_testing ()
endif ()

add_library ( libos SHARED IMPORTED )

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    message(INFO " - building for Linux....")
    include(${PROJECT_SOURCE_DIR}/CMake_Build_Platforms/Linux.cmake)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    message(INFO " - building for Windows....")
    include(${PROJECT_SOURCE_DIR}/CMake_Build_Platforms/Windows.cmake)
else()
    message(FATAL_ERROR "system not supported")
endif()

add_executable ( LunaScript ${app_source} )
target_link_libraries(LunaScript PRIVATE liblunascript libos)
target_include_directories (LunaScript PRIVATE  ${PROJECT_SOURCE_DIR}/libs/include STATIC )
target_include_directories (LunaScript PRIVATE  ${PROJECT_SOURCE_DIR}/src/share STATIC )
if ( ${COMPILER_TESTING} )
    gtest_discover_tests ( LunaScript )
    target_link_libraries(LunaScript PRIVATE GTest::gtest_main liblunascript libos)
endif ()