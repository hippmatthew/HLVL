project(PhysicsPlus::Core)

set(HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/context.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/context_decl.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/device.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/interface.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/interface_decl.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/iwindow.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/iwindow_decl.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/settings.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/settings_decl.hpp
)

set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/context.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/iwindow.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/settings.cpp
)

add_library(physp.core OBJECT ${HEADERS} ${SOURCES})