project(HLVL::linalg)

set(LINALG_INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/mat.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vec.hpp
)

set(LINALG_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/mat.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/vec.cpp
)

add_library(hlvl.linalg OBJECT ${LINALG_INCLUDES} ${LINALG_SOURCES})

target_include_directories(hlvl.linalg PRIVATE ${CMAKE_SOURCE_DIR})