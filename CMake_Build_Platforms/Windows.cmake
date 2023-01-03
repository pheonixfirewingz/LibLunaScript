set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_CXX_EXTENSIONS OFF)

set_target_properties(libos PROPERTIES
  IMPORTED_IMPLIB  ${PROJECT_SOURCE_DIR}/libs/binarys/libos.lib
  IMPORTED_LOCATION  ${PROJECT_SOURCE_DIR}/libs/binarys/libos.dll
)

add_library ( liblunascript SHARED ${compiler_source} )
target_include_directories ( liblunascript PRIVATE ${PROJECT_SOURCE_DIR}/src/share/rapidjson/include STATIC )
target_include_directories (liblunascript PRIVATE  ${PROJECT_SOURCE_DIR}/src/share STATIC )
target_include_directories (liblunascript PRIVATE  ${PROJECT_SOURCE_DIR}/libs/include STATIC )