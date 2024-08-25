#include "tests/test_classes.hpp"

#include <catch2/catch_test_macros.hpp>

#include <iostream>

TEST_CASE( "new_entity", "[unit][controller]" )
{
  pp::ECSController controller;

  pp::Entity entity;
  for (unsigned long i = 0; i < 5; ++i)
    entity = controller.new_entity();

  CHECK( entity == 4 );
}

TEST_CASE( "register_components", "[unit][controller]" )
{
  pp::ECSController controller;

  bool success = true;
  try
  {
    controller.register_components<Test1, Test2>();
  }
  catch (std::exception& e)
  {
    std::cout << "register_components: " << e.what() << '\n';
    success = false;
  }
  catch( ... )
  {
    std::cout << "register_components: unknown\n";
    success = false;
  }

  CHECK( success );
}

TEST_CASE( "unregister_components", "[unit][controller]" )
{
  pp::ECSController controller;
  controller.register_components<Test1, Test2>();

  bool success = true;
  try
  {
    controller.unregister_components<Test1, Test2>();
  }
  catch (std::exception& e)
  {
    std::cout << "unregister_components: " << e.what() << '\n';
    success = false;
  }
  catch( ... )
  {
    std::cout << "unregister_components: unknown\n";
    success = false;
  }

  CHECK( success );
}

TEST_CASE( "add_components", "[unit][controller]" )
{
  pp::ECSController controller;
  controller.register_components<Test1, Test2>();
  pp::Entity entity = controller.new_entity();

  pp::Signature signature = 0x11ul;

  bool success = true;
  try
  {
    controller.add_components(entity, Test1(), Test2());
  }
  catch (std::exception& e)
  {
    std::cout << "add_components: " << e.what() << '\n';
    success = false;
  }
  catch( ... )
  {
    std::cout << "add_components: unknown\n";
    success = false;
  }

  CHECK( success );
}