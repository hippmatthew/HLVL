#include "test_classes.hpp"

#include <catch2/catch_test_macros.hpp>

#include <iostream>

TEST_CASE( "new_entity", "[unit][controller]" )
{
  hlvl::ECSController controller;

  hlvl::Entity entity;
  for (unsigned long i = 0; i < 5; ++i)
    entity = controller.new_entity();

  CHECK( entity == 4 );
}

TEST_CASE( "register_components", "[unit][controller]" )
{
  hlvl::ECSController controller;
  auto entity = controller.new_entity();

  bool success = true;
  try
  {
    controller.register_components<Test1, Test2>()
              .add_components(entity, Test1());
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
  hlvl::ECSController controller;
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
  hlvl::ECSController controller;
  controller.register_components<Test1, Test2>();
  hlvl::Entity entity = controller.new_entity();

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

TEST_CASE( "remove_components", "[unit][controller]" )
{
  hlvl::ECSController controller;
  controller.register_components<Test1, Test2>();
  hlvl::Entity entity = controller.new_entity();
  controller.add_components(entity, Test1(), Test2());

  bool success = true;
  try
  {
    controller.remove_components<Test1, Test2>(entity);
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

TEST_CASE( "register_systems", "[unit][controller]" )
{
  hlvl::ECSController controller;

  bool success = true;
  try
  {
    controller.register_systems<System1, System2>();
  }
  catch (std::exception& e)
  {
    std::cout << "register_systems: " << e.what() << '\n';
    success = false;
  }
  catch( ... )
  {
    std::cout << "register_systems: unknown\n";
    success = false;
  }

  CHECK( success );
}

TEST_CASE( "unregister_systems", "[unit][controller]" )
{
  hlvl::ECSController controller;
  controller.register_systems<System1, System2>();

  bool success = true;
  try
  {
    controller.unregister_systems<System1, System2>();
  }
  catch (std::exception& e)
  {
    std::cout << "unregister_systems: " << e.what() << '\n';
    success = false;
  }
  catch( ... )
  {
    std::cout << "unregister_systems: unknown\n";
    success = false;
  }

  CHECK( success );
}

TEST_CASE( "add_entities_to_system", "[unit][controller]" )
{
  hlvl::ECSController controller;
  controller.register_components<int>()
            .register_systems<System1>()
            .require_components<System1, int>();

  bool success = true;
  try
  {
    auto entity = controller.new_entity();
    controller.add_components(entity, 1);

    controller.add_to_system<System1>({ entity });
  }
  catch (std::exception& e)
  {
    std::cout << "add_entities_to_system: " << e.what() << '\n';
    success = false;
  }
  catch( ... )
  {
    std::cout << "add_entities_to_system: unknown\n";
    success = false;
  }

  CHECK( success );
}

TEST_CASE( "remove_entities_from_system", "[unit][controller]" )
{
  hlvl::ECSController controller;
  controller.register_components<int>()
            .register_systems<System1>()
            .require_components<System1, int>();

  auto entity = controller.new_entity();
  controller.add_components(entity, 1)
            .add_to_system<System1>({ entity });

  bool success = true;
  try
  {
    controller.remove_from_system<System1>({ entity });
  }
  catch (std::exception& e)
  {
    std::cout << "remove_entities_from_system: " << e.what() << '\n';
    success = false;
  }
  catch( ... )
  {
    std::cout << "remove_entities_from_system: unknown\n";
    success = false;
  }

  CHECK( success );
}