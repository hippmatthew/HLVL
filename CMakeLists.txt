cmake_minimum_required(VERSION 3.20.0)

option(HLVL_GENERATE_HEADERS "Generates Headers. Requires zsh." ON)
option(HLVL_BUILD_TESTS "Builds tests. Requires Catch2." ON)
option(HLVL_DEBUG "Enables debug flags and optimization level to 0" ON)

if(NOT CMAKE_CXX_STANDARD GREATER_EQUAL 20)
  set(CMAKE_CXX_STANDARD 20)
endif()

if (HLVL_DEBUG)
  set(CMAKE_BUILD_TYPE Debug)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
else()
  set(CMAKE_BUILD_TYPE Release)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto")
endif()

if (HLVL_BUILD_TESTS)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DTESTS")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DTESTS")
endif()

project(HLVL
  VERSION 0.62.1
  LANGUAGES CXX
)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

set(HLVL_INCL_DIR ${CMAKE_BINARY_DIR}/include)
set(HLVL_LIB_DIR ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${HLVL_LIB_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${HLVL_LIB_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
    include_directories(
      ${HLVL_INCL_DIR}
      ${CMAKE_SOURCE_DIR}
      /opt/homebrew/include
      /usr/local/include
    )

    link_directories(
      ${HLVL_LIB_DIR}
      /opt/homebrew/lib
      /usr/local/lib
    )
  else()
    include_directories(
      ${HLVL_INCL_DIR}
      ${CMAKE_SOURCE_DIR}
      /usr/local/include
    )

    link_directories(
      ${HLVL_LIB_DIR}
      /usr/local/lib
    )
  endif()
else()
  include_directories(
    ${HLVL_INCL_DIR}
    ${CMAKE_SOURCE_DIR}
  )

  link_directories(
    ${HLVL_LIB_DIR}
  )
endif()

set(LINKS
  Vulkan::Vulkan
  glfw
)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)

add_library(hlvl STATIC
  $<TARGET_OBJECTS:hlvl.core>
  $<TARGET_OBJECTS:hlvl.windows>
)

target_link_libraries(hlvl PRIVATE ${LINKS})

if(HLVL_GENERATE_HEADERS)
  set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
  set(BUILD_DIR ${CMAKE_BINARY_DIR})

  add_custom_target(HLVLHeaders ALL
    COMMAND zsh ${CMAKE_SOURCE_DIR}/generate_headers.zsh ${HLVL_VERSION} ${SRC_DIR} ${BUILD_DIR}
  )
endif()

if(HLVL_BUILD_TESTS)
  add_subdirectory(${CMAKE_SOURCE_DIR}/tests)
endif()