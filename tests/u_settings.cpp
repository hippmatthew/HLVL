#include "src/core/include/settings.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "modify_settings", "[unit][settings]" ) {
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

    hlvl_settings.max_flight_frames = 3;
    CHECK( hlvl_settings.max_flight_frames == 3 );
  }

  SECTION( "reset" ) {
    hlvl::Settings::reset_to_default();

    CHECK( hlvl_settings.window_width == 1280 );
    CHECK( hlvl_settings.window_height == 720 );
    CHECK( hlvl_settings.window_title == "HLVL");
    CHECK( hlvl_settings.application_name == "HLVL Application" );
    CHECK( hlvl_settings.application_version == hlvl_make_version(1, 0, 0) );
    CHECK( hlvl_settings.max_flight_frames == 2 );
    CHECK( hlvl_settings.format == vk::Format::eB8G8R8A8Srgb );
    CHECK( hlvl_settings.color_space == vk::ColorSpaceKHR::eSrgbNonlinear );
    CHECK( hlvl_settings.extent == vk::Extent2D{ 1280, 720 } );
  }
}