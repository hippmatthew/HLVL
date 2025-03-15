#include "src/core/include/settings.hpp"
#include "vulkan/vulkan_enums.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "settings", "[unit][settings]" ) {
  SECTION( "modify" ) {
    hlvl_settings.window_width = 10;
    CHECK( hlvl_settings.window_width == 10 );

    hlvl_settings.window_height = 10;
    CHECK( hlvl_settings.window_height == 10 );

    hlvl_settings.window_title = "Test";
    CHECK( hlvl_settings.window_title == "Test" );

    hlvl_settings.application_name = "Test";
    CHECK( hlvl_settings.application_name == "Test" );

    hlvl_settings.application_version = 69420;
    CHECK( hlvl_settings.application_version == 69420 );

    hlvl_settings.buffer_mode = hlvl::BufferMode::DoubleBuffer;
    CHECK( hlvl_settings.buffer_mode == hlvl::BufferMode::DoubleBuffer );

    hlvl_settings.format = vk::Format::eA2R10G10B10UintPack32;
    CHECK( hlvl_settings.format == vk::Format::eA2R10G10B10UintPack32 );

    hlvl_settings.depth_format = vk::Format::eA2B10G10R10UintPack32;
    CHECK( hlvl_settings.depth_format == vk::Format::eA2B10G10R10UintPack32 );

    hlvl_settings.color_space = vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear;
    CHECK( hlvl_settings.color_space == vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear );

    hlvl_settings.present_mode = vk::PresentModeKHR::eFifo;
    CHECK( hlvl_settings.present_mode == vk::PresentModeKHR::eFifo );

    hlvl_settings.extent = vk::Extent2D{ 10, 10 };
    CHECK( hlvl_settings.extent == vk::Extent2D{ 10, 10 } );

    hlvl_settings.background_color = std::array<float, 4>{ 0.0, 1.0, 0.0, 1.0 };
    CHECK( hlvl_settings.background_color == std::array<float, 4>{ 0.0, 1.0, 0.0, 1.0 } );
  }

  SECTION( "reset" ) {
    hlvl::Settings::reset_to_default();

    CHECK( hlvl_settings.window_width == 1280 );
    CHECK( hlvl_settings.window_height == 720 );
    CHECK( hlvl_settings.window_title == "HLVL");

    CHECK( hlvl_settings.application_name == "HLVL Application" );
    CHECK( hlvl_settings.application_version == hlvl_make_version(1, 0, 0) );

    CHECK( hlvl_settings.buffer_mode == hlvl::BufferMode::TripleBuffer );
    CHECK( hlvl_settings.format == vk::Format::eB8G8R8A8Srgb );
    CHECK( hlvl_settings.depth_format == vk::Format::eD32Sfloat );
    CHECK( hlvl_settings.color_space == vk::ColorSpaceKHR::eSrgbNonlinear );
    CHECK( hlvl_settings.extent == vk::Extent2D{ 1280, 720 } );
    CHECK( hlvl_settings.background_color == std::array<float, 4>{ 0.0, 0.0, 0.0, 1.0 } );
  }
}