project(HLVL::hlvl)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

set(CORE_INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/context.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/settings.hpp
)

set(CORE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/context.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/settings.cpp
)

add_library(hlvl SHARED
  ${CORE_INCLUDES}
  ${CORE_SOURCES}
)

target_include_directories(hlvl PUBLIC
  ${CMAKE_SOURCE_DIR}
  /usr/local/include
)

target_link_directories(hlvl PUBLIC /usr/local/lib)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" AND ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
  target_include_directories(hlvl PUBLIC /opt/homebrew/include)
  target_link_directories(hlvl PUBLIC /opt/homebrew/lib)
endif()

target_link_libraries(hlvl PRIVATE
  Vulkan::Vulkan
  glfw
)