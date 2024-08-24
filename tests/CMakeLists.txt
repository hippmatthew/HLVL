project(PhysicsPlus::Tests)

find_package(Catch2 3 REQUIRED)

set(SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/test_classes.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/end_to_end.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/u_allocator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/u_context.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/u_settings.cpp
)

add_executable(test ${SOURCES})

target_link_libraries(test PRIVATE
  physp
  Catch2::Catch2
)