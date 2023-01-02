cmake_minimum_required ( VERSION 3.16.3 )
project ( libLunaScript CXX )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )

# options
set ( LIB_MODE OFF )
set ( DEBUG_MODE ON )
set ( FUZZING OFF )
set ( COMPILER_TESTING OFF )

# options end
if ( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    set ( CMAKE_COMPILER_NUMBER 0 )
elseif ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
    set ( CMAKE_COMPILER_NUMBER 1 )
elseif ( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
    set ( CMAKE_COMPILER_NUMBER 2 )
else ()
    message ( FATAL_ERROR "compiler not supported" )
endif ()

configure_file ( src/Cmake.h.in ../src/Cmake.h @ONLY )
include_directories ( ${PROJECT_SOURCE_DIR}/src/share/rapidjson/include STATIC )
include_directories ( ${PROJECT_SOURCE_DIR}/src/share STATIC )

set ( compiler_source
    src/compiler/front/Parser.cpp
    src/compiler/front/Json.cpp
    src/compiler/back/IRCodeGen.cpp
    src/compiler/back/ByteCodeGen.cpp
    src/compiler/back/DebugGen.cpp
    src/compiler/IO.cpp
    src/compiler/Main.cpp
)

if ( ${COMPILER_TESTING} )
    set ( compiler_source ${compiler_source}
        src/compiler/Test.cpp
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
set_property ( TARGET libos PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/libs/binarys/liblibos.so )

if ( ${LIB_MODE} )
    add_library ( LunaScript SHARED ${compiler_source} )
else ()
    add_executable ( LunaScript ${compiler_source} )

    if ( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
    else ()
        target_include_directories ( LunaScript PRIVATE /usr/lib/llvm-15/include STATIC )
        target_compile_options ( LunaScript PRIVATE -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS )
        target_link_options ( LunaScript PRIVATE -L/usr/lib/llvm-15/lib -lLLVM-15 )
    endif ()

    if ( ${COMPILER_TESTING} )
        target_link_libraries ( LunaScript PRIVATE GTest::gtest_main libos )
    else ()
        target_link_libraries ( LunaScript PRIVATE libos )
    endif ()
endif ()

set_property ( TARGET LunaScript PROPERTY CXX_STANDARD 20 )
target_include_directories ( LunaScript PRIVATE ${PROJECT_SOURCE_DIR}/libs/include STATIC )
target_compile_options ( LunaScript PRIVATE -Wall -Wextra -Werror )

if ( ${DEBUG_MODE} )
    if ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
        if ( ${COMPILER_TESTING} )
            gtest_discover_tests ( LunaScript )
            target_compile_options ( LunaScript PRIVATE -O2 -g3 -static-libgcc -static-libstdc++ )
        else ()
            target_compile_options ( LunaScript PRIVATE -static-libgcc -static-libstdc++ -g3 -O0 )
        endif ()
    else ()
        if ( ${FUZZING} )
            target_compile_options ( LunaScript PRIVATE -fsanitize=fuzzer,undefined )
            target_link_options ( LunaScript PRIVATE -fsanitize=fuzzer,undefined )
        else ()
            if ( ${COMPILER_TESTING} )
                gtest_discover_tests ( LunaScript )
                target_compile_options ( LunaScript PRIVATE -O2 -g3 )
            else ()
                target_compile_options ( LunaScript PRIVATE -g3 -O0 )
            endif ()
        endif ()
    endif ()
else ()
    target_compile_options ( LunaScript PRIVATE -O3 -g0 )
endif ()