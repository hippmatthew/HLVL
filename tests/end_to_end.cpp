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

  std::vector<hlvl::Vertex> vertices = {
    {{ 0.5, 0.5, 0.5 }, { 1.0, 1.0 }},
    {{ 0.5, -0.5, 0.5 }, { 1.0, 0.0 }},
    {{ -0.5, -0.5, 0.5 }, { 0.0, 0.0 }},
    {{ -0.5, 0.5, 0.5 }, { 0.0, 1.0 }},
    {{ 0.5, 0.5, -0.5 }, { 1.0, 1.0 }},
    {{ 0.5, -0.5, -0.5 }, { 1.0, 0.0 }},
    {{ -0.5, -0.5, -0.5 }, { 0.0, 0.0 }},
    {{ -0.5, 0.5, -0.5 }, { 0.0, 1.0 }}
  };

  std::vector<unsigned int> indices = {
    0, 1, 2, 2, 3, 0,
    4, 0, 3, 3, 7, 4,
    5, 4, 7, 7, 6, 5,
    1, 5, 6, 6, 2, 1,
    4, 5, 1, 1, 0, 4,
    6, 7, 3, 3, 2, 6
  };

  hlvl::Context context;

  REQUIRE( context.get_window() != nullptr );
  REQUIRE( *context.get_instance() != nullptr );
  REQUIRE( *context.get_surface() != nullptr );
  REQUIRE( *context.get_physicalDevice() != nullptr );
  REQUIRE( *context.get_device() != nullptr );
  REQUIRE( !context.get_queueFamilies().empty() );

  hlvl::Resource rectMatrices(Matrices{});
  hlvl::Resource wallMatrices(Matrices{});
  Color color;

  hlvl_materials.create(
    hlvl::Material::builder("rectangle")
      .add_shader(vk::ShaderStageFlagBits::eVertex, "shaders/rectangle.vert.spv")
      .add_shader(vk::ShaderStageFlagBits::eFragment, "shaders/rectangle.frag.spv")
      .add_resource(hlvl::ResourceInfo{
        .type     = hlvl::BufferType::Uniform,
        .stages   = vk::ShaderStageFlagBits::eVertex,
        .resource = &rectMatrices
      })
      .add_constants(sizeof(Color), &color)
  );

  REQUIRE( hlvl_materials.count() == 1 );
  REQUIRE( *hlvl_materials["rectangle"].get_gLayout() != nullptr );
  REQUIRE( *hlvl_materials["rectangle"].get_gPipeline() != nullptr );
  REQUIRE( !hlvl_materials["rectangle"].get_dsLayouts().empty() );
  REQUIRE( *hlvl_materials["rectangle"].get_descriptorPool() != nullptr );
  REQUIRE( !hlvl_materials["rectangle"].get_descriptorSets().empty() );
  REQUIRE( *hlvl_materials["rectangle"].get_texMemory() == nullptr );
  REQUIRE( hlvl_materials["rectangle"].get_images().empty() );
  REQUIRE( hlvl_materials["rectangle"].get_views().empty() );
  REQUIRE( hlvl_materials["rectangle"].get_samplers().empty() );
  REQUIRE( *hlvl_materials["rectangle"].get_bufMemory() != nullptr );
  REQUIRE( !hlvl_materials["rectangle"].get_buffers().empty() );
  REQUIRE( hlvl_materials["rectangle"].get_constantsSize() != 0 );
  REQUIRE( hlvl_materials["rectangle"].get_constants() != nullptr );

  hlvl_materials.create(
    hlvl::Material::builder("wall")
      .add_shader(vk::ShaderStageFlagBits::eVertex, "shaders/wall.vert.spv")
      .add_shader(vk::ShaderStageFlagBits::eFragment, "shaders/wall.frag.spv")
      .add_texture("../tests/img/test.png")
      .add_resource({
        .type     = hlvl::BufferType::Uniform,
        .stages   = vk::ShaderStageFlagBits::eVertex,
        .resource = &wallMatrices
      })
  );

  hlvl_objects.add(
    hlvl::Object::builder()
      .add_vertices({
        {{ -1.0, -1.0, 0.0 }, { 0.0, 0.0 }},
        {{ -1.0, 1.0, 0.0 }, { 0.0, 1.0 }},
        {{ 1.0, 1.0, 0.0 }, { 1.0, 1.0 }},
        {{ 1.0, -1.0, 0.0 }, { 1.0, 0.0 }}
      })
      .add_indices({ 0, 1, 2, 2, 3, 0 })
      .add_material("wall")
  );

  hlvl_objects.add(
    hlvl::Object::builder()
      .add_vertices(vertices)
      .add_indices(indices)
      .add_material("rectangle")
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