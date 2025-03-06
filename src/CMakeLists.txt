find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/linalg)

add_library(hlvl SHARED
  $<TARGET_OBJECTS:hlvl.core>
  $<TARGET_OBJECTS:hlvl.linalg>
)

target_link_directories(hlvl PUBLIC /usr/local/lib)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" AND ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
  target_link_directories(hlvl PUBLIC /opt/homebrew/lib)
endif()

target_link_libraries(hlvl PRIVATE
  Vulkan::Vulkan
  glfw
)