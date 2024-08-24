#include "tests/test_classes.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "windowed", "[unit][context]" )
{
  reset_settings();

  pp_general_settings.vk_layers.emplace_back("VK_LAYER_KHRONOS_validation");
  pp_general_settings.draw_window = true;

  SECTION( "default" )
  {
    pp::Context context;
    context.initialize();
  }

  SECTION( "manual" )
  {
    pp_general_settings.add_instance_extensions(pp::windows::GLFW::instance_extensions());
    pp_general_settings.add_device_extensions(pp::windows::GLFW::device_extensions());

    pp::Context context;
    context.set_interface<pp::windows::GLFW>()
            .initialize();
  }
}

TEST_CASE( "shell_only", "[unit][context]" )
{
  reset_settings();

  pp::Context context;
  context.initialize();
}