project(HLVL::core)

set(CORE_INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/context.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/settings.hpp
)

set(CORE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/context.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/settings.cpp
)

add_library(hlvl.core OBJECT ${CORE_INCLUDES} ${CORE_SOURCES})

target_include_directories(hlvl.core PRIVATE
  ${CMAKE_SOURCE_DIR}
  /usr/local/include
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" AND ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
  target_include_directories(hlvl.core PRIVATE /opt/homebrew/include)
endif()