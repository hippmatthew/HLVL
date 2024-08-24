cmake_minimum_required(VERSION 3.20.0)

option(PHYSP_GENERATE_HEADERS "Generates Headers. Requires zsh." ON)
option(PHYSP_BUILD_TESTS "Builds tests. Requires Catch2." ON)
option(PHYSP_DEBUG "Enables debug flags and optimization level to 0" OFF)

if(NOT CMAKE_CXX_STANDARD GREATER_EQUAL 20)
  set(CMAKE_CXX_STANDARD 20)
endif()

if (PHYSP_DEBUG)
  set(CMAKE_BUILD_TYPE Debug)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
else()
  set(CMAKE_BUILD_TYPE Release)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto")
endif()

if (PHYSP_BUILD_TESTS)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DTESTS")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DTESTS")
endif()

project(PhysicsPlus
  VERSION 0.42.2
  LANGUAGES CXX
)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

set(PHYSP_INCL_DIR ${CMAKE_BINARY_DIR}/include)
set(PHYSP_LIB_DIR ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PHYSP_LIB_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PHYSP_LIB_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
    include_directories(
      ${PHYSP_INCL_DIR}
      ${CMAKE_SOURCE_DIR}
      /opt/homebrew/include
      /usr/local/include
    )

    link_directories(
      ${PHYSP_LIB_DIR}
      /opt/homebrew/lib
      /usr/local/lib
    )
  else()
    include_directories(
      ${PHYSP_INCL_DIR}
      ${CMAKE_SOURCE_DIR}
      /usr/local/include
    )

    link_directories(
      ${PHYSP_LIB_DIR}
      /usr/local/lib
    )
  endif()
else()
  include_directories(
    ${PHYSP_INCL_DIR}
    ${CMAKE_SOURCE_DIR}
  )

  link_directories(
    ${PHYSP_LIB_DIR}
  )
endif()

set(LINKS
  Vulkan::Vulkan
  glfw
)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)

add_library(physp STATIC
  $<TARGET_OBJECTS:physp.core>
  $<TARGET_OBJECTS:physp.windows>
)

target_link_libraries(physp PRIVATE ${LINKS})

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

if (CMAKE_EXPORT_COMPILE_COMMANDS)
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
  )
endif()