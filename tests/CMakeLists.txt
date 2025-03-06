project(HLVL::tests)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(Catch2 3 REQUIRED)

set(TESTS_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/end_to_end.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/u_mat.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/u_settings.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/u_vec.cpp
)

add_executable(test ${TESTS_SOURCES})

target_include_directories(test PRIVATE
  ${CMAKE_SOURCE_DIR}
  /usr/local/include
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" AND ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
  target_include_directories(test PRIVATE /opt/homebrew/include)
  set_target_properties(test PROPERTIES LINK_FLAGS "-rpath /usr/local/lib")
endif()

target_link_libraries(test PRIVATE
  hlvl
  Catch2::Catch2
)