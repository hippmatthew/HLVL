project(PhysicsPlus::tests)

find_package(Catch2 3 REQUIRED)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" AND ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
  include_directories(
    ${PHYSP_INCL_DIR}
    ${CMAKE_SOURCE_DIR}
    /opt/homebrew/include
  )

  link_directories(
    ${PHYSP_LIB_DIR}
    /opt/homebrew/lib
  )
else()
  include_directories(
    ${PHYSP_INCL_DIR}
    ${CMAKE_SOURCE_DIR}
  )

  link_directories(
    ${PHYSP_LIB_DIR}
  )
endif()

set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/u_settings.cpp
)

add_executable(test ${SOURCES})

target_link_libraries(test PRIVATE
  physp
  Catch2::Catch2
)