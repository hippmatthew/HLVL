#include "test_classes.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "windowed", "[unit][context]" )
{
  reset_settings();

  hlvl_general_settings.vk_layers.emplace_back("VK_LAYER_KHRONOS_validation");
  hlvl_general_settings.draw_window = true;

  SECTION( "default" )
  {
    hlvl::Context context;
    context.initialize();
  }

  SECTION( "manual" )
  {
    hlvl_general_settings.add_instance_extensions(hlvl::windows::GLFW::instance_extensions());
    hlvl_general_settings.add_device_extensions(hlvl::windows::GLFW::device_extensions());

    hlvl::Context context;
    context.set_interface<hlvl::windows::GLFW>()
            .initialize();
  }
}

TEST_CASE( "shell_only", "[unit][context]" )
{
  reset_settings();

  hlvl::Context context;
  context.initialize();
}