function (add_local_library LIB_NAME LIB_TYPE)
    if(${LIB_TYPE} STREQUAL  "shared")
        message(INFO "[BUILDING] getting dynamic link info for - ${LIB_NAME}...")
        add_library(${LIB_NAME} SHARED IMPORTED)

        if(CMAKE_SYSTEM_NAME MATCHES Linux)
            set_property ( TARGET ${LIB_NAME} PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/libs/binarys/lib${LIB_NAME}.so )
        elseif(CMAKE_SYSTEM_NAME MATCHES WindowsStore)
             set_target_properties(${LIB_NAME} PROPERTIES
              IMPORTED_IMPLIB  ${PROJECT_SOURCE_DIR}/libs/binarys/${LIB_NAME}.lib
              IMPORTED_LOCATION  ${PROJECT_SOURCE_DIR}/libs/binarys/${LIB_NAME}.dll
            )
        elseif(CMAKE_SYSTEM_NAME MATCHES Windows)
            set_target_properties(${LIB_NAME} PROPERTIES
              IMPORTED_IMPLIB  ${PROJECT_SOURCE_DIR}/libs/binarys/${LIB_NAME}.lib
              IMPORTED_LOCATION  ${PROJECT_SOURCE_DIR}/libs/binarys/${LIB_NAME}.dll
            )
        elseif(CMAKE_SYSTEM_NAME MATCHES Android)
        else()
            message(FATAL_ERROR " system not supported")
        endif()
    else()
        message(INFO "[BUILDING] getting static link info for - ${LIB_NAME}...")
        add_library( ${LIB_NAME} STATIC IMPORTED)
        if(CMAKE_SYSTEM_NAME MATCHES Linux)
            set_property ( TARGET ${LIB_NAME} PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/libs/binarys/lib${LIB_NAME}.so )
        elseif(CMAKE_SYSTEM_NAME MATCHES WindowsStore)
              set_target_properties(${LIB_NAME} PROPERTIES
                IMPORTED_IMPLIB  ${PROJECT_SOURCE_DIR}/libs/binarys/${LIB_NAME}.lib
              )
        elseif(CMAKE_SYSTEM_NAME MATCHES Windows)
              set_target_properties(${LIB_NAME} PROPERTIES
                IMPORTED_IMPLIB  ${PROJECT_SOURCE_DIR}/libs/binarys/${LIB_NAME}.lib
              )
        elseif(CMAKE_SYSTEM_NAME MATCHES Android)
        else()
            message(FATAL_ERROR "system not supported")
        endif()
    endif()
endfunction()