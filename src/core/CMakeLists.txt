project(HLVL::Core)

set(HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/allocation.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/allocator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/components.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/components_decl.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/context.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/context_decl.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/device.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ecscontroller.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ecscontroller_decl.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/entitymanager.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/interface.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/interface_decl.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/iwindow.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/resource.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/resource_decl.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/settings.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/settings_decl.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/signature.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/stagingbuffer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/systems.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/systems_decl.hpp
)

set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/allocation.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/allocator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/context.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ecscontroller.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/entitymanager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/iwindow.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/resource.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/settings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/signature.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/stagingbuffer.cpp
)

add_library(hlvl.core OBJECT ${HEADERS} ${SOURCES})