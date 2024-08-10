cmake_minimum_required(VERSION 3.20.0)

option(PHYSP_GENERATE_HEADERS "Generates Headers. Requires zsh." ON)
option(PHYSP_BUILD_TESTS "Builds tests. Requires Catch2." ON)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

project(PhysicsPlus
  VERSION 0.0.0
  LANGUAGES CXX
)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

set(PHYSP_INCL_DIR ${CMAKE_BINARY_DIR}/include)
set(PHYSP_LIB_DIR ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PHYSP_LIB_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PHYSP_LIB_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(${CMAKE_SOURCE_DIR}/src)

include_directories(
  ${CMAKE_SOURCE_DIR}
  /opt/homebrew/include
  /usr/local/include
)

link_directories(
  /opt/homebrew/lib
  /usr/local/lib
)

add_library(physp STATIC
  $<TARGET_OBJECTS:physp.core>
)

target_link_libraries(physp PRIVATE
  Vulkan::Vulkan
  glfw3
)

if(PHYSP_GENERATE_HEADERS)
  set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
  set(BUILD_DIR ${CMAKE_BINARY_DIR})

  add_custom_target(PhysicsPlusHeaders ALL
    COMMAND zsh ${CMAKE_SOURCE_DIR}/generate_headers.zsh ${PhysicsPlus_VERSION} ${SRC_DIR} ${BUILD_DIR}
  )
endif()

if(PHYSP_BUILD_TESTS)
  add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
endif()