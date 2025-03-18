#define hlvl_tests
#include "src/core/include/context.hpp"
#include "src/core/include/materials.hpp"
#include "src/core/include/objects.hpp"
#include "src/core/include/settings.hpp"
#include "src/linalg/include/mat.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <cmath>
#include <numbers>

TEST_CASE( "end_to_end", "[endtoend]" ) {
  static float aspectRatio = static_cast<float>(hlvl_settings.extent.width) / hlvl_settings.extent.height;
  static float height = 0.2f * tanf(std::numbers::pi / 4);
  static float width = aspectRatio * height;

  struct Sphere {
    float radius;
    float emiss_str;
    la::vec<3> position;
    la::vec<3> color;
    la::vec<3> emiss_color;
  };

  struct SphereData {
    unsigned int sphereCount = 5;
    Sphere spheres[5] = {
      { 20, 0, { 0.0, -21.0, 0.0 }, { 0.1, 0.05, 0.9 }, { 0.0, 0.0, 0.0 } },
      { 1, 0, { 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 } },
      { 0.7, 0, { -3.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 0.0, 0.0, 0.0 } },
      { 0.4, 0, { 2.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0, 0.0 } },
      { 5, 5, { 5.0, 5.0, 5.0 }, { 0.0, 0.0, 0.0 }, { 0.65, 0.3, 0.1 }}
    };
  };

  struct CameraInfo {
    la::mat<4> view = la::mat<4>::view({ 0, 0, -3 }, { 0, 0, 0 });
  };

  struct PushConstants {
    la::vec<2, unsigned int> screenDims = { hlvl_settings.extent.width, hlvl_settings.extent.height };
    la::vec<3> npDims = { width, height, 0.1f };
  };

  hlvl::Context context;

  const unsigned int size = hlvl_settings.extent.width * hlvl_settings.extent.height;

  hlvl::Resource sphereData(SphereData{});
  hlvl::Resource cameraInfo(CameraInfo{});
  PushConstants constants;

  hlvl_materials.create(hlvl::Material::builder("camera")
    .add_shader(vk::ShaderStageFlagBits::eCompute, "shaders/camera.comp.spv")
    .add_shader(vk::ShaderStageFlagBits::eVertex, "shaders/camera.vert.spv")
    .add_shader(vk::ShaderStageFlagBits::eFragment, "shaders/camera.frag.spv")
    .add_canvas()
    .add_storage(vk::ShaderStageFlagBits::eCompute, &sphereData)
    .add_uniform(vk::ShaderStageFlagBits::eCompute, &cameraInfo)
    .add_constants(sizeof(PushConstants), &constants)
    .compute_space((hlvl_settings.extent.width + 15) / 15, (hlvl_settings.extent.height + 15) / 15, 1)
  );

  hlvl_objects.add(hlvl::Object::builder()
    .add_material("camera")
    .add_model("../tests/dat/camera.obj")
  );

  context.run([&cameraInfo]{
    static std::chrono::time_point currTime = std::chrono::steady_clock::now();
    static std::chrono::time_point prevTime = currTime;
    static float elapsedTime = 0;

    currTime = std::chrono::steady_clock::now();
    elapsedTime += std::chrono::duration<float>(currTime - prevTime).count();
    prevTime = currTime;

    cameraInfo = CameraInfo{ .view =
      la::mat<4>::view({ 5 * cosf(0.6 * elapsedTime), 0, 5 * sinf(0.6 * elapsedTime) }, { 0, 0, 0 })
    };
  });
}