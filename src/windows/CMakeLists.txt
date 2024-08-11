project(PhysicsPlus::Windows)

set(HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/glfw.hpp
)

set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/glfw.cpp
)

add_library(physp.windows OBJECT ${HEADERS} ${SOURCES})