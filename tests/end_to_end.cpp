#define hlvl_tests
#include "src/core/include/context.hpp"
#include "src/core/include/materials.hpp"
#include "src/core/include/objects.hpp"

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

  REQUIRE( hlvl_materials.count() != 0 );
  CHECK( hlvl_materials["triangle"].get_gLayout() != nullptr );
  CHECK( hlvl_materials["triangle"].get_gLayout() != nullptr );

  hlvl_objects.add(
    hlvl::Object::builder()
      .add_vertices({
        {{ 0.0, 0.5, 0.0 }},
        {{ -0.5, -0.5, 0.0 }},
        {{ 0.5, -0.5, 0.0 }}
      })
      .add_indices({ 0, 1, 2 })
      .add_material("triangle")
  );

  REQUIRE( hlvl_objects.count() != 0 );
  CHECK( hlvl_objects.get_object(0).get_memory() != nullptr );
  for (const auto& buffer : hlvl_objects.get_object(0).get_buffers())
    CHECK( buffer != nullptr );

  unsigned int counter = 0;

  hlvl_loop_start {

    ++counter;

  } hlvl_loop_end(counter == 5);

  CHECK( counter == 5 );
}