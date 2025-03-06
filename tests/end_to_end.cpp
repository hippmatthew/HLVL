#define hlvl_tests
#include "src/core/include/context.hpp"
#include "src/core/include/materials.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "end_to_end", "[endtoend]" ) {
  hlvl::Context context;

  REQUIRE( context.get_window() != nullptr );
  REQUIRE( context.get_instance() != nullptr );
  REQUIRE( context.get_surface() != nullptr );
  REQUIRE( context.get_physicalDevice() != nullptr );
  REQUIRE( context.get_device() != nullptr );
  REQUIRE( !context.get_queueFamilies().empty() );

  hlvl_materials.create(
    hlvl::Material::builder("triangle")
      .add_shader(vk::ShaderStageFlagBits::eVertex, "shaders/triangle.vert.spv")
      .add_shader(vk::ShaderStageFlagBits::eFragment, "shaders/triangle.frag.spv")
  );

  REQUIRE( hlvl_materials["triangle"].get_gLayout() != nullptr );
  REQUIRE( hlvl_materials["triangle"].get_gLayout() != nullptr );

  unsigned int counter = 0;

  hlvl_loop_start {

    ++counter;

  } hlvl_loop_end(counter == 5);

  CHECK( counter == 5 );
}