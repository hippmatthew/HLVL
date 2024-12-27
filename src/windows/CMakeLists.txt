project(HLVL::Windows)

set(HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/glfw.hpp
)

set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/glfw.cpp
)

add_library(hlvl.windows OBJECT ${HEADERS} ${SOURCES})