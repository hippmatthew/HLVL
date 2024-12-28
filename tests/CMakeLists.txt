project(HLVL::Tests)

find_package(Catch2 3 REQUIRED)

set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/test_classes.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/end_to_end.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/u_allocator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/u_componentmanager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/u_context.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/u_controller.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/u_entitymanager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/u_settings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/u_systemmanager.cpp
)

add_executable(test ${SOURCES})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  set_target_properties(test PROPERTIES
    LINK_FLAGS "-rpath /usr/local/lib"
  )
endif()

target_link_libraries(test PRIVATE
  hlvl
  Catch2::Catch2
)