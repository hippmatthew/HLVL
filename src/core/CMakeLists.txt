project(HLVL::core)

set(CORE_INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/context.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/materials.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/objects.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/renderer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/resource.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/settings.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vertex.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vkfactory.hpp
)

set(CORE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/context.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/materials.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/objects.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/renderer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/settings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/vertex.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/vkfactory.cpp
)

add_library(hlvl.core OBJECT ${CORE_INCLUDES} ${CORE_SOURCES})

target_include_directories(hlvl.core PRIVATE
  ${CMAKE_SOURCE_DIR}
  /usr/local/include
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" AND ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
  target_include_directories(hlvl.core PRIVATE /opt/homebrew/include)
endif()

target_compile_definitions(hlvl.core PRIVATE
  "GLFW_INCLUDE_NONE"
  "VULKAN_HPP_NO_CONSTRUCTORS"
  "hlvl_engine_version=hlvl_make_version(${HLVL_VERSION_MAJOR}, ${HLVL_VERSION_MINOR}, ${HLVL_VERSION_PATCH})"
)