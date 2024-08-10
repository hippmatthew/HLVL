project(PhysicsPlus::Core)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
  include_directories(
    ${CMAKE_SOURCE_DIR}
    /usr/local/include
    /opt/homebrew/include
  )
else()
  include_directories(
    ${CMAKE_SOURCE_DIR}
  )
endif()

set(PHYSP_CORE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/core/context.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core/settings.cpp
)

add_library(physp.core OBJECT ${PHYSP_CORE_SOURCES})