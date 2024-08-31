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

  unsigned long finalvalue1 = 0, finalvalue2 = 0;

  bool success = true;
  try
  {
    pp::Context context;
    context.initialize()
      .add_keybind(pp::Key::a, KeyTest::pressA)
      .add_keybind({ pp::Key::a, pp::Key::b }, KeyTest::pressAB);

    auto& allocator = context.allocator();
    auto& controller = context.ecs_controller();

    controller.register_components<pp::Resource<Test1>, int>()
              .register_systems<System1, System2>();

    controller.require_components<System1, pp::Resource<Test1>>()
              .require_components<System2, pp::Resource<Test1>, int>();

    auto entity1 = controller.new_entity();
    auto entity2 = controller.new_entity();

    controller.add_components(entity1, pp::Resource<Test1>(Test1()))
              .add_components(entity2, pp::Resource<Test1>(Test1()), 2);

    controller.add_to_system<System1>({ entity1, entity2 })
              .add_to_system<System2>({ entity2 });

    allocator.new_allocation({ pp::Locality::device, { &counter } });
    allocator.new_allocation({ pp::Locality::host, { &controller.component<pp::Resource<Test1>>(entity1) } });
    allocator.new_allocation({ pp::Locality::device, { &controller.component<pp::Resource<Test1>>(entity2) } });

    auto system1 = controller.system<System1>();
    auto system2 = controller.system<System2>();

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

    system1->run();
    system2->run();

    ++iteration;

    pp_loop_end( iteration == iterations );

    allocator.wait();

    finalvalue1 = (*controller.component<pp::Resource<Test1>>(entity1)).a;
    finalvalue2 = (*controller.component<pp::Resource<Test1>>(entity2)).a;
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

  REQUIRE( success );
  CHECK( *counter == iterations );
  CHECK( keyTest.a == true );
  CHECK( keyTest.both == true );
  CHECK( pp::windows::GLFW::resized == true );
  CHECK( finalvalue1 == 1000 );
  CHECK( finalvalue2 == 1022 );
}

int main()
{
  return Catch::Session().run();
}