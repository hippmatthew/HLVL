#define hlvl_tests
#include "src/core/include/context.hpp"
#include "src/core/include/materials.hpp"
#include "src/core/include/objects.hpp"
#include "src/linalg/include/mat.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <cmath>
#include <numbers>

TEST_CASE( "end_to_end", "[endtoend]" ) {
  struct Colors {
    la::vec<3> colors[256];
  };

  // struct Matrices {
  //   la::mat<4> model = la::mat<4>::identity();
  //   la::mat<4> view = la::mat<4>::view({ 0, 0, 2 }, { 0, 0, 0 }, { 0, 1, 0 });
  //   la::mat<4> projection = la::mat<4>::projection(
  //     3.14159 / 2, hlvl_settings.extent.width / static_cast<float>(hlvl_settings.extent.height), 0.1, 10
  //   );
  // };

  struct PushConstants {
    la::vec<3> color = { 0.0, 1.0, 0.0 };
  };

  Colors colors;
  for (int i = 0; i < 256; ++i)
    colors.colors[i] = { (float)(i < 85), (float)(i > 84 && i < 170), (float)(i > 169) };

  PushConstants cubeConstants;

  hlvl::Context context;

  REQUIRE( context.get_window() != nullptr );
  REQUIRE( *context.get_instance() != nullptr );
  REQUIRE( *context.get_surface() != nullptr );
  REQUIRE( *context.get_physicalDevice() != nullptr );
  REQUIRE( *context.get_device() != nullptr );
  REQUIRE( !context.get_queueFamilies().empty() );

  hlvl::Resource cubeStorage(colors);

  hlvl_materials.create(
    hlvl::Material::builder("cube")
      .add_shader(vk::ShaderStageFlagBits::eVertex, "shaders/cube.vert.spv")
      .add_shader(vk::ShaderStageFlagBits::eFragment, "shaders/cube.frag.spv")
      .add_texture("../tests/dat/eggplant.png")
      .add_storage(vk::ShaderStageFlagBits::eFragment, &cubeStorage)
      .add_constants(sizeof(PushConstants), &cubeConstants)
  );

  REQUIRE( *hlvl_materials["cube"].get_layout() != nullptr );
  REQUIRE( *hlvl_materials["cube"].get_gPipeline() != nullptr );
  REQUIRE( hlvl_materials["cube"].get_dsLayouts().size() == 2 );
  REQUIRE( *hlvl_materials["cube"].get_dsPool() != nullptr );
  REQUIRE( hlvl_materials["cube"].get_sets().size() == 2 );
  REQUIRE( *hlvl_materials["cube"].get_texMem() != nullptr );
  REQUIRE( hlvl_materials["cube"].get_images().size() == 1 );
  REQUIRE( hlvl_materials["cube"].get_views().size() == 1 );
  REQUIRE( hlvl_materials["cube"].get_samplers().size() == 1 );
  REQUIRE( *hlvl_materials["cube"].get_sMem() != nullptr );
  REQUIRE( hlvl_materials["cube"].get_sBufs().size() == hlvl_settings.buffer_mode );
  REQUIRE( hlvl_materials["cube"].get_constantsSize() == sizeof(PushConstants) );
  REQUIRE( hlvl_materials["cube"].get_constants() != nullptr );

  hlvl_objects.add(
    hlvl::Object::builder()
      .add_model("../tests/dat/cube.obj")
      .add_material("cube")
  );

  REQUIRE( *hlvl_objects.get_object(0).get_memory() != nullptr );
  REQUIRE( hlvl_objects.get_object(0).get_buffers().size() == 2 );

  context.run([&cubeConstants](){
    static std::chrono::time_point currTime = std::chrono::steady_clock::now();
    static std::chrono::time_point prevTime = currTime;
    static float elapsedTime = 0;

    currTime = std::chrono::steady_clock::now();
    elapsedTime += std::chrono::duration<float>(currTime - prevTime).count();
    prevTime = currTime;

    cubeConstants = {{
      (float)cos(1.5 * elapsedTime + 4 * std::numbers::pi / 3) + 1.0f,
      (float)cos(1.5 * elapsedTime) + 1.0f,
      (float)cos(1.5 * elapsedTime + 2 * std::numbers::pi / 3) + 1.0f
    }};
  });
}