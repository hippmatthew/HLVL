#define hlvl_tests
#include "src/core/include/context.hpp"
#include "src/core/include/materials.hpp"
#include "src/core/include/objects.hpp"
#include "src/linalg/include/mat.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>

TEST_CASE( "end_to_end", "[endtoend]" ) {
  struct Matrices {
    la::mat<4> model = la::mat<4>::identity();
    la::mat<4> view = la::mat<4>::view({ 0, 0, 5 }, { 0, 0, 0 }, { 0, 1, 0 });
    la::mat<4> projection = la::mat<4>::projection(
      3.14159 / 2, hlvl_settings.extent.width / static_cast<float>(hlvl_settings.extent.height), 0.1, 10
    );
  };

  struct Color {
    la::vec<3> value = { 0, 1, 0 };
  };

  hlvl::Context context;

  REQUIRE( context.get_window() != nullptr );
  REQUIRE( *context.get_instance() != nullptr );
  REQUIRE( *context.get_surface() != nullptr );
  REQUIRE( *context.get_physicalDevice() != nullptr );
  REQUIRE( *context.get_device() != nullptr );
  REQUIRE( !context.get_queueFamilies().empty() );

  hlvl::Resource rectMatrices(Matrices{});
  hlvl::Resource wallMatrices(Matrices{ .model = la::mat<4>::scale({ 3, 2, 1 }) });
  Color color;

  hlvl_materials.create(
    hlvl::Material::builder("cube")
      .add_shader(vk::ShaderStageFlagBits::eVertex, "shaders/cube.vert.spv")
      .add_shader(vk::ShaderStageFlagBits::eFragment, "shaders/cube.frag.spv")
      .add_texture("../tests/dat/eggplant.png")
      .add_resource(hlvl::ResourceInfo{
        .type     = hlvl::BufferType::Uniform,
        .stages   = vk::ShaderStageFlagBits::eVertex,
        .resource = &rectMatrices
      })
      .add_constants(sizeof(Color), &color)
  );

  REQUIRE( hlvl_materials.count() == 1 );
  REQUIRE( *hlvl_materials["cube"].get_gLayout() != nullptr );
  REQUIRE( *hlvl_materials["cube"].get_gPipeline() != nullptr );
  REQUIRE( !hlvl_materials["cube"].get_dsLayouts().empty() );
  REQUIRE( *hlvl_materials["cube"].get_descriptorPool() != nullptr );
  REQUIRE( !hlvl_materials["cube"].get_descriptorSets().empty() );
  REQUIRE( *hlvl_materials["cube"].get_texMemory() != nullptr );
  REQUIRE( !hlvl_materials["cube"].get_images().empty() );
  REQUIRE( !hlvl_materials["cube"].get_views().empty() );
  REQUIRE( !hlvl_materials["cube"].get_samplers().empty() );
  REQUIRE( *hlvl_materials["cube"].get_bufMemory() != nullptr );
  REQUIRE( !hlvl_materials["cube"].get_buffers().empty() );
  REQUIRE( hlvl_materials["cube"].get_constantsSize() != 0 );
  REQUIRE( hlvl_materials["cube"].get_constants() != nullptr );

  hlvl_materials.create(
    hlvl::Material::builder("wall")
      .add_shader(vk::ShaderStageFlagBits::eVertex, "shaders/wall.vert.spv")
      .add_shader(vk::ShaderStageFlagBits::eFragment, "shaders/wall.frag.spv")
      .add_resource({
        .type     = hlvl::BufferType::Uniform,
        .stages   = vk::ShaderStageFlagBits::eVertex,
        .resource = &wallMatrices
      })
      .add_constants(sizeof(Color), &color)
  );

  hlvl_objects.add(
    hlvl::Object::builder()
      .add_vertices({
        {{ -0.5, -0.5, 0.0 }, { 0.0, 0.0 }},
        {{ -0.5, 0.5, 0.0 }, { 0.0, 1.0 }},
        {{ 0.5, 0.5, 0.0 }, { 1.0, 1.0 }},
        {{ 0.5, -0.5, 0.0 }, { 1.0, 0.0 }}
      })
      .add_indices({ 0, 1, 2, 2, 3, 0 })
      .add_material("wall")
  );

  hlvl_objects.add(
    hlvl::Object::builder()
      .add_model("../tests/dat/cube.obj")
      .add_material("cube")
  );

  auto prevTime = std::chrono::steady_clock::now();
  auto currTime = prevTime;
  double elapsedTime = 0.0;

  context.run([&prevTime, &currTime, &elapsedTime, &rectMatrices, &color]() {
    currTime = std::chrono::steady_clock::now();
    elapsedTime += std::chrono::duration<double>(currTime - prevTime).count();
    prevTime = currTime;

    rectMatrices = Matrices{ .model =
      la::mat<4>::translation({ static_cast<float>(3 * sin(elapsedTime)), 0, static_cast<float>(3 * cos(elapsedTime)) }) *
      la::mat<4>::rotation({ 0, -2 * static_cast<float>(elapsedTime), 0 })
    };

    color = {{
      static_cast<float>(cos(elapsedTime + 4 * 3.141579 / 3) + 1),
      static_cast<float>(cos(elapsedTime) + 1),
      static_cast<float>(cos(elapsedTime + 2 * 3.141579 / 3) + 1)
    }};
  });
}