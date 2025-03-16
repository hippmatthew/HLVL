project(HLVL::obj)

set(OBJ_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/parser.hpp)
set(OBJ_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp)

add_library(hlvl.obj OBJECT ${OBJ_INCLUDES} ${OBJ_SOURCES})

target_include_directories(hlvl.obj PRIVATE ${CMAKE_SOURCE_DIR})