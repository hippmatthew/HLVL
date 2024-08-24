#include "tests/test_classes.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>

#include <exception>
#include <iostream>

KeyTest * KeyTest::instance = nullptr;

TEST_CASE( "run_test", "[endtoend]" )
{
  reset_settings();

  pp_general_settings.vk_layers.emplace_back("VK_LAYER_KHRONOS_validation");
  pp_general_settings.draw_window = true;

  pp::Resource<unsigned int> counter(0, pp::ResourceType::uniform);
  unsigned long iterations = 1000;
  KeyTest keyTest;

  bool success = true;
  try
  {
    pp::Context context;
    context.initialize()
      .add_keybind(pp::Key::a, KeyTest::pressA)
      .add_keybind({ pp::Key::a, pp::Key::b }, KeyTest::pressAB);

    auto& allocator = context.allocator();

    allocator.new_allocation({ pp::Locality::device, { &counter }});

    unsigned int iteration = 0;

    pp_loop_start

    counter = *counter + 1;

    if (iteration == 0)
      pp::windows::GLFW::key_callback(pp::windows::GLFW::p_window, GLFW_KEY_A, 0, GLFW_PRESS, 0);
    else if (iteration == 1)
      pp::windows::GLFW::key_callback(pp::windows::GLFW::p_window, GLFW_KEY_A, 0, GLFW_RELEASE, 0);
    else if (iteration == 333)
      pp::windows::GLFW::resize_callback(pp::windows::GLFW::p_window, 500, 500);
    else if (iteration == 666)
      pp::windows::GLFW::key_callback(pp::windows::GLFW::p_window, GLFW_KEY_A, 0, GLFW_PRESS, 0);
    else if (iteration == 667)
      pp::windows::GLFW::key_callback(pp::windows::GLFW::p_window, GLFW_KEY_B, 0, GLFW_PRESS, 0);
    else if (iteration == 668)
      pp::windows::GLFW::key_callback(pp::windows::GLFW::p_window, GLFW_KEY_B, 0, GLFW_RELEASE, 0);
    else if (iteration == 669)
      pp::windows::GLFW::key_callback(pp::windows::GLFW::p_window, GLFW_KEY_A, 0, GLFW_RELEASE, 0);

    ++iteration;

    pp_loop_end( (iteration == iterations) );

    allocator.wait();
  }
  catch( std::exception& e)
  {
    std::cout << "run_test: " << e.what() << '\n';
    success = false;
  }
  catch( ... )
  {
    std::cout << "run_test: unknown\n";
    success = false;
  }

  CHECK( success );
  CHECK( *counter == iterations );
  CHECK( keyTest.a == true );
  CHECK( keyTest.both == true );
  CHECK( pp::windows::GLFW::resized == true );
}

int main()
{
  return Catch::Session().run();
}